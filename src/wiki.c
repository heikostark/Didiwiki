/*
 *  DidiWiki - a small lightweight wiki engine.
 *
 *  Copyright 2004 Matthew Allum <mallum@o-hand.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

/* */
#include "didi.h"
#include "wikitext.h"

#include "libfile.c"

static char* CssData = STYLESHEET;
static int globalsub;
static char* oldpage;

static char datadir[512] = { 0 };
static char categorydir[524] = { 0 };
static char includedir[524] = { 0 };
static char imagedir[524] = { 0 };
static char secretdir[524] = { 0 };
static char trashdir[524] = { 0 };

static int  html_found = 0;
static int  math_found = 0;
static int  code_found = 0;

WikiPageList** /* only pages */
wiki_get_pages ( int  *n_pages, char *url, char *expr )
{
    WikiPageList  **pages;
    struct dirent **namelist;
    int             n, i = 0;
    struct stat     st;

    n = scandir ( url, &namelist, 0, ( void * ) changes_compar );
    pages = malloc ( sizeof ( WikiPageList* ) *n );
    while ( n-- ) {
        if ( ( namelist[n]->d_name ) [0] == '.' || !strcasecmp ( namelist[n]->d_name, "styles.css" ) )
            goto cleanup;
        /*		if ((expr != NULL) && (*expr != '*'))
        		{ 			*/ /* Super Simple Search */ /*
char *data = NULL;
if ((data = file_read(namelist[n]->d_name)) != NULL)
if (strcasestr(data, expr) == NULL)
	if (strcasecmp(namelist[n]->d_name, expr) != 0)
		goto cleanup;
} */
        stat ( namelist[n]->d_name, &st );
        /* ignore anything but regular readable files */
        if ( S_ISREG ( st.st_mode ) && access ( namelist[n]->d_name, R_OK ) == 0 ) {
            pages[i]        = malloc ( sizeof ( WikiPageList ) );
            pages[i]->name  = strdup ( namelist[n]->d_name );
            pages[i]->mtime = st.st_mtime;
            i++;
        }
    cleanup:
        free ( namelist[n] );
    }
    *n_pages = i;
    free ( namelist );
    if ( i==0 ) return NULL;
    return pages;
}

WikiPageList** /* pages and inside pages */
wiki_get_searchpages ( int  *n_pages, char *url, char *expr )
{
    WikiPageList  **pages;
    struct dirent **namelist;
    int             n, i = 0;
    struct stat     st;

    n = scandir ( url, &namelist, 0, ( void * ) alphasort /*changes_compar*/ );
    pages = malloc ( sizeof ( WikiPageList* ) *n );
    while ( n-- ) {
        if ( ( namelist[n]->d_name ) [0] == '.' || !strcasecmp ( namelist[n]->d_name, "styles.css" ) ) goto cleanup;
        if ( ( expr != NULL ) && ( *expr != '*' ) ) {
            /* Super Simple Search inside the wiki files */
            char *data = NULL;
            if ( ( data = file_read ( namelist[n]->d_name ) ) != NULL )
                if ( strcasestr ( data, expr ) == NULL )
                    if ( strcasecmp ( namelist[n]->d_name, expr ) != 0 ) goto cleanup;
        }
        stat ( namelist[n]->d_name, &st );
        /* ignore anything but regular readable files */
        if ( S_ISREG ( st.st_mode ) && access ( namelist[n]->d_name, R_OK ) == 0 ) {
            pages[i]        = malloc ( sizeof ( WikiPageList ) );
            pages[i]->name  = strdup ( namelist[n]->d_name );
            pages[i]->mtime = st.st_mtime;
            i++;
        }
    cleanup:
        free ( namelist[n] );
    }
    *n_pages = i;
    free ( namelist );
    if ( i==0 ) return NULL;
    return pages;
}

/* ******************************************************************************** */

int
wiki_redirect ( HttpResponse *res, char *location ) /* Redirect */
{
    int   header_len = strlen ( location ) + 14;
    char *header = alloca ( sizeof ( char ) *header_len );

    snprintf ( header, header_len, "Location: %s\r\n", location );
    http_response_append_header ( res, header );
    http_response_printf ( res, "<html>\n<p>Redirect to %s</p>\n</html>\n", location );
    http_response_set_status ( res, 302, "Moved Temporarily" );
    http_response_send ( res );
    exit ( 0 );
}

/* ******************************************************************************** */

#include "libstring.c"
#include "libshow.c"

/* ******************************************************************************** */

void
wiki_print_data_as_html ( HttpResponse *res, char *raw_page_data )
{
    char *p = raw_page_data;	    /* accumalates non marked up text */
    char *q = NULL, *link = NULL; /* temporary scratch stuff */
    char *line = NULL;
    int   line_len;
    int   i, j, skip_chars;

    /* flags, mainly for open tag states */
    int bold_on      = 0;
    int italic_on    = 0;
    int superscript_on = 0;
    int subscript_on = 0;

    int open_para    = 0;
    int pre_on       = 0;
    int term_on      = 0;
    int description_on = 0;
    int table_on     = 0;

#define NUM_LIST_TYPES 14

    struct {
        char *ident;
        int  depth;
        char *tag;
    } listtypes[] = {
        { "•", 0, "ul" },
        { "o", 0, "ul style='list-style-type:circle'" },
        { "*c", 0, "ul style='list-style-type:circle'" },
        { "*d", 0, "ul style='list-style-type:disc'" },
        { "*s", 0, "ul style='list-style-type:square'" },
        { "*n", 0, "ul style='list-style-type:none'" },
        { "*", 0, "ul" },
        { "#I", 0, "ol style='list-style-type:upper-roman'" },
        { "#i", 0, "ol style='list-style-type:lower-roman'" },
        { "#A", 0, "ol style='list-style-type:upper-latin'" },
        { "#a", 0, "ol style='list-style-type:lower-latin'" },
        { "#1", 0, "ol style='list-style-type:decimal'" },
        { "#01", 0, "ol style='list-style-type:decimal-leading-zero'" },
        { "#", 0, "ol" }
    };

    q = p;
    /* p accumalates non marked up text, q is just a pointer
     * to the end of the current line - used by below func.
     */

    while ( ( line = get_line_from_string ( &q, &line_len ) ) ) {
        int   header_level = 0;
        char *line_start   = line;
        int   skip_to_content = 0;
        /* process any initial wiki chars at line beginning */
        if ( pre_on && !isspace ( *line ) && *line != '\0' ) {
            /* close any preformatting if already on*/
            http_response_printf ( res, "\n</pre>\n" ) ;
            pre_on = 0;
        }
        /* Handle ordered & unordered list, code is a bit mental.. */
        for ( i=0; i<NUM_LIST_TYPES; i++ ) {
            /* extra checks avoid bolding */
            if ( !strncmp ( line, listtypes[i].ident, strlen ( listtypes[i].ident ) ) &&
                    ( !strncmp ( line+strlen ( listtypes[i].ident ), listtypes[i].ident, strlen ( listtypes[i].ident ) ) ||
                      * ( line+strlen ( listtypes[i].ident ) ) == ' ' ) ) {
                int item_depth = 0;
                if ( listtypes[!i].depth ) {
                    for ( j=0; j<listtypes[!i].depth; j++ ) http_response_printf ( res, "</%s>\n", listtypes[!i].tag );
                    listtypes[!i].depth = 0;
                }
                while ( !strncmp ( line, listtypes[i].ident, strlen ( listtypes[i].ident ) ) ) {
                    line=line+strlen ( listtypes[i].ident );
                    item_depth++;
                }
                if ( item_depth < listtypes[i].depth ) {
                    for ( j = 0; j < ( listtypes[i].depth - item_depth ); j++ ) http_response_printf ( res, "</%s>\n", listtypes[i].tag );
                } else {
                    for ( j = 0; j < ( item_depth - listtypes[i].depth ); j++ ) http_response_printf ( res, "<%s>\n", listtypes[i].tag );
                }
                http_response_printf ( res, "<li>" );
                listtypes[i].depth = item_depth;
                skip_to_content = 1;
            } else if ( listtypes[i].depth && !listtypes[!i].depth ) {
                /* close current list */
                for ( j=0; j<listtypes[i].depth; j++ ) http_response_printf ( res, "</%s>\n", listtypes[i].tag );
                listtypes[i].depth = 0;
            }
        }

        if ( skip_to_content )
            goto line_content; /* skip parsing any more initial chars */

        /* Tables */

        if ( *line == '|' ) {
            if ( table_on==0 ) http_response_printf ( res, "<table class='wikitable' cellspacing='0' cellpadding='4'>\n" );
            line++;
            http_response_printf ( res, "<tr><td>" );
            table_on = 1;
            goto line_content;
        } else {
            if ( table_on ) {
                http_response_printf ( res, "</table>\n" );
                table_on = 0;
            }
        }

        /* $ include */

        if ( *line == '$' ) {
            line++;
            int len = strlen ( line );
            if ( !strncasecmp ( line, "category:", 9 ) ) {
                char *newpage = "";
                newpage = malloc ( sizeof ( char ) * ( 255 ) );
                memset ( newpage, 0, sizeof ( char ) * ( 255 ) );
                strncpy ( newpage, line+9, len );
                line--;
                *line = '\0';
                wiki_show_included_category_results_page ( res,newpage );
            } else if ( !strncasecmp ( line, "search:", 7 ) ) {
                char *newpage = "";
                newpage = malloc ( sizeof ( char ) * ( 255 ) );
                memset ( newpage, 0, sizeof ( char ) * ( 255 ) );
                strncpy ( newpage, line+7, len );
                line--;
                *line = '\0';
                wiki_show_included_search_results_page ( res,datadir,newpage );
            } else if ( !strncasecmp ( line, "redirect:", 9 ) ) {
                char *newpage = "";
                newpage = malloc ( sizeof ( char ) * ( 255 ) );
                memset ( newpage, 0, sizeof ( char ) * ( 255 ) );
                strncpy ( newpage, line+9, len );
                line--;
                *line = '\0';
                if ( !strcasecmp ( oldpage,newpage ) ) {
                    http_response_printf ( res, "<b>!! Recursion detected !!</b>" ) ;
                } else {
                    http_response_printf ( res, "<b>Redirected page</b> (to: <a href='%s'>%s</a>)\n\n",newpage,newpage ) ;
                    globalsub = 0;
                    if ( access ( newpage, R_OK ) == 0 ) 	/* page exists */
                        wiki_show_subpage ( res,newpage );
                }
            } else {
                char *newpage = "";
                newpage = malloc ( sizeof ( char ) * ( 255 ) );
                memset ( newpage, 0, sizeof ( char ) * ( 255 ) );
                strcpy ( newpage, line );
                line--;
                *line = '\0';
                chdir ( includedir );
                if ( access ( newpage, R_OK ) == 0 ) /* page exists */ wiki_show_subpage ( res,newpage );
                else {
                    http_response_printf ( res, "<b>!! Missign Subpage " ) ;
                    http_response_printf ( res, newpage ) ;
                    http_response_printf ( res, " !!</b>" ) ;
                }
                chdir ( ".." );
            }
        }

        /* pre formated  */
        /* Begin - space and return */
        if ( ( isspace ( *line ) || *line == '\0' ) && ( html_found+math_found+code_found == 0 ) ) {
            int n_spaces = 0;

            while ( isspace ( *line ) ) {
                line++;
                n_spaces++;
            }
            if ( *line == '\0' ) { /* empty line - para */
                if ( pre_on ) {
                    http_response_printf ( res, "\n" ) ;
                    continue;
                } else if ( open_para ) {
                    http_response_printf ( res, "\n</p>\n<p>\n" ) ;
                } else {
                    http_response_printf ( res, "\n<p>\n" ) ;
                    open_para = 1;
                }
            } else { /* starts with space so Pre formatted, see above for close */
                if ( !pre_on ) http_response_printf ( res, "<pre>\n" ) ;
                pre_on = 1;
                line = line - ( n_spaces - 1 );
                /* rewind so extra spaces they matter to pre */
                http_response_printf ( res, "%s\n", line );
                continue;
            }
        } else if ( *line == ':' ) {
            line++;
            if ( open_para ) {
                if ( !description_on ) http_response_printf ( res, "</p><dd>" );
            } else {
                if ( !description_on ) http_response_printf ( res, "<dd>" );
            }
            description_on += 1;
            p = line;
        } else if ( *line == ';' ) {
            line++;
            if ( open_para ) {
                if ( !term_on ) http_response_printf ( res, "</p><dt>" );
            } else {
                if ( !term_on ) http_response_printf ( res, "<dt>" );
            }
            term_on += 1;
            p = line;
        } else if ( *line == '=' ) {
            while ( *line == '=' ) {
                header_level++;
                line++;
            }
            if ( open_para ) {
                http_response_printf ( res, "</p><h%d>", header_level );
            } else {
                http_response_printf ( res, "<h%d>", header_level );
            }
            p = line;
        } else if ( *line == '-' && * ( line+1 ) == '-' ) {
            /* rule */
            if ( open_para ) {
                http_response_printf ( res, "</p><hr /><p>\n" );
            } else {
                http_response_printf ( res, "<hr />\n" );
            }
            while ( *line == '-' ) line++;
        }

    line_content:
        /* now process rest of the line */
        p = line;
        while ( *line != '\0' ) {
            if ( *line == '!' && !isspace ( * ( line+1 ) ) ) {
                /* escape next word - skip it */
                *line = '\0';
                http_response_printf ( res, "%s", p );
                p = ++line;
                while ( *line != '\0' && !isspace ( *line ) ) line++;
                if ( *line == '\0' ) continue;
            }
            /* Check for link */
            else if ( ( link = check_for_link ( line, &skip_chars ) ) != NULL ) {
                http_response_printf ( res, "%s", p );
                http_response_printf ( res, "%s", link );
                line += skip_chars;
                p = line;
                continue;
            }
            /* check for html, math, ... */
            else if ( ( link = check_for_html ( line, &skip_chars ) ) != NULL ) {
                http_response_printf ( res, "%s", p );
                http_response_printf ( res, "%s", link );
                line += skip_chars;
                p = line;
                continue;
            } 
            /* Bold */
            else if ( *line == '*' ) {
                if ( line_start != line && !is_wiki_format_char_or_space ( * ( line-1 ) ) && !bold_on ) {
                    line++;
                    continue;
                }
                if ( ( isspace ( * ( line+1 ) ) && !bold_on ) ) {
                    line++;
                    continue;
                }
                *line = '\0';
                http_response_printf ( res, "%s%s\n", p, bold_on ? "</b>" : "<b>" );
                bold_on ^= 1; /* reset flag */
                p = line+1;
            }
            /* Superscript */
            else if ( *line == '^' ) {
                if ( line_start != line && !is_wiki_format_char_or_space ( * ( line-1 ) ) && !superscript_on ) {
                    line++;
                    continue;
                }
                if ( isspace ( * ( line+1 ) ) && !superscript_on ) {
                    line++;
                    continue;
                }
                *line = '\0';
                http_response_printf ( res, "%s%s\n", p, superscript_on ? "</sup>" : "<sup>" );
                superscript_on ^= 1; /* reset flag */
                p = line+1;
            }
            /* Subscript */
            else if ( *line == '_' ) {
                if ( line_start != line && !is_wiki_format_char_or_space ( * ( line-1 ) ) && !subscript_on ) {
                    line++;
                    continue;
                }
                if ( isspace ( * ( line+1 ) ) && !subscript_on ) {
                    line++;
                    continue;
                }
                *line = '\0';
                http_response_printf ( res, "%s%s\n", p, subscript_on ? "</sub>" : "<sub>" );
                subscript_on ^= 1; /* reset flag */
                p = line+1;
            }
            /* Italic */
            else if ( *line == '/' ) {
                if ( line_start != line && !is_wiki_format_char_or_space ( * ( line-1 ) ) && !italic_on ) {
                    line++;
                    continue;
                }
                if ( isspace ( * ( line+1 ) ) && !italic_on ) {
                    line++;
                    continue;
                }
                /* crude path detection */
                if ( line_start != line && isspace ( * ( line-1 ) ) && !italic_on ) {
                    char *tmp   = line+1;
                    int slashes = 0;
                    /* Hack to escape out file paths */
                    while ( *tmp != '\0' && !isspace ( *tmp ) ) {
                        if ( *tmp == '/' ) slashes++;
                        tmp++;
                    }
                    if ( slashes > 1 || ( slashes == 1 && * ( tmp-1 ) != '/' ) )  {
                        line = tmp;
                        continue;
                    }
                }
                if ( * ( line+1 ) == '/' ) line++; 	/* escape out common '//' - eg urls */
                else {
                    /* italic */
                    *line = '\0';
                    http_response_printf ( res, "%s%s", p, italic_on ? "</i>" : "<i>" );
                    italic_on ^= 1; /* reset flag */
                    p = line+1;
                }
            }
            /* Table */
            else if ( *line == '|' && table_on ) { /* table column */
                *line = '\0';
                http_response_printf ( res, "%s", p );
                http_response_printf ( res, "</td><td>\n" );
                p = line+1;
            }
            line++;
        } /* next word */
        /* accumalated text left over */
        if ( *p != '\0' ) http_response_printf ( res, "%s", p );
        /* close any html tags that could be still open */

        for ( i=0; i<NUM_LIST_TYPES; i++ ) {
            if ( listtypes[i].depth ) http_response_printf ( res, "</li>" );
        }

        if ( table_on ) http_response_printf ( res, "</td></tr>\n" );

        if ( header_level ) {
            if ( open_para ) {
                http_response_printf ( res, "</h%d><p>\n", header_level );
            } else {
                http_response_printf ( res, "</h%d>\n", header_level );
            }
        }
        if ( description_on ) {
            if ( open_para ) {
                http_response_printf ( res, "</dd><p>\n" );
            } else {
                http_response_printf ( res, "</dd>\n" );
            }
            description_on--;
        }

        if ( term_on ) {
            if ( open_para ) {
                http_response_printf ( res, "</dt><p>\n" );
            } else {
                http_response_printf ( res, "</dt>\n" );
            }
            term_on--;
        }
        /* else http_response_printf(res, "\n"); */
    } /* next line */
    /* clean up anything thats still open */
    if ( pre_on ) http_response_printf ( res, "</pre>\n" );
    /* close any open lists */
    for ( i=0; i<NUM_LIST_TYPES; i++ ) {
        for ( j=0; j<listtypes[j].depth; j++ ) http_response_printf ( res, "</%s>\n",listtypes[i].tag );
    }

    /* close any open paras */
    if ( open_para ) http_response_printf ( res, "</p>\n" );
    /* tables */
    if ( table_on ) http_response_printf ( res, "</table>\n" );
}

/* ******************************************************************************** */

void
wiki_handle_http_request ( HttpRequest *req )
{
    HttpResponse *res      = http_response_new ( req );
    char         *page     = http_request_get_path_info ( req );
    char         *command  = http_request_get_query_string ( req );
    char         *wikitext = "";
    char         *file     = "";
    char         *from     = "";
    char         *to       = "";
    char         *name     = NULL;
    int          len       = 0;

    util_dehttpize ( page ); 	/* remove any encoding on the requested page name.  */
    /* For slash */
    if ( !strcasecmp ( page, "/" ) ) {
        if ( access ( "WikiHome", R_OK ) != 0 ) wiki_redirect ( res, "/WikiHome?create" );
        page = "/WikiHome";
    } else if ( !strcasecmp ( page, "/styles.css" ) ) {
        /*  Return CSS page */
        http_response_set_content_type ( res, "text/css" );
        http_response_printf ( res, "%s", CssData );
        http_response_send ( res );
        exit ( 0 );
    } else if ( !strcasecmp ( page, "/favicon.ico" ) ) {
        /*  Return favicon */
        http_response_set_content_type ( res, "image/ico" );
        http_response_set_data ( res, FaviconData, FaviconDataLen );
        http_response_send ( res );
        exit ( 0 );
    } else if ( !strncasecmp ( page, "/images/",8 ) ) {
        /*  Return images */
        chdir ( imagedir );
        page = page + 8;
        if ( access ( page, R_OK ) == 0 ) { /* image exists */
            int len2 = strlen ( page );
            int found = 0;
            if ( !strncasecmp ( page+len2-4, ".gif", 4 ) ) {
                http_response_set_content_type ( res, "image/gif" );
                found = 1;
            } else if ( !strncasecmp ( page+len2-4, ".png", 4 ) ) {
                http_response_set_content_type ( res, "image/png" );
                found = 1;
            } else if ( !strncasecmp ( page+len2-4, ".jpg", 4 ) ) {
                http_response_set_content_type ( res, "image/jpeg" );
                found = 1;
            } else if ( !strncasecmp ( page+len2-5, ".jpeg", 5 ) ) {
                http_response_set_content_type ( res, "image/jpeg" );
                found = 1;
            } else if ( !strncasecmp ( page+len2-4, ".ico", 4 ) ) {
                http_response_set_content_type ( res, "image/ico" );
                found = 1;
            } else if ( !strncasecmp ( page+len2-4, ".tif", 4 ) ) {
                http_response_set_content_type ( res, "image/tiff" );
                found = 1;
            } else if ( !strncasecmp ( page+len2-5, ".tiff", 5 ) ) {
                http_response_set_content_type ( res, "image/tiff" );
                found = 1;
            }
            if ( found ) {
                file = file_read_bin ( page,&len );
                http_response_set_data ( res, file, len );
                http_response_send ( res );
                free ( file );
            }
        }
        chdir ( ".." );
        exit ( 0 );
    }
    /* skip slash */
    page = page + 1;
    oldpage = page;
    /* second char */
    if ( strchr ( page, '/' ) ) {
        strncpy ( page, "WikiHome\0", 9 );
        if ( access ( page, R_OK ) == 0 ) {	/* page exists */
            wikitext = file_read ( page );
            wiki_show_page ( res, wikitext, page );
        }
    } else if ( !strcasecmp ( page, "Changes" ) ) {
        wiki_show_changes_page ( res );
    } else if ( !strcasecmp ( page, "Search" ) ) {
        wiki_show_search_results_page ( res, http_request_param_get ( req, "expr" ) );
    } else if ( !strcasecmp ( page, "Find" ) ) {
        wiki_show_find_results_page ( res, http_request_param_get ( req, "expr" ) );
    } else if ( !strcasecmp ( page, "Category" ) ) {
        wiki_show_category_results_page ( res, http_request_param_get ( req, "expr" ) );
    } else if ( !strcasecmp ( page, "Login" ) ) {
        if ( ( wikitext = http_request_param_get ( req, "title" ) ) != NULL ) {
            chdir ( secretdir );
            if ( access ( wikitext, R_OK ) == 0 ) {	/* page exists */
                file = file_read ( wikitext );
                wiki_show_secret_page ( res, file, wikitext );
            } else {
                http_response_set_status ( res, 401, "Unauthorized" );
                http_response_printf ( res, "<html><body><br /><center>Wrong login!</center></body></html>\n" );
                http_response_send ( res );
            }
        } else {
            wiki_show_login_page ( res );
        }
    } else if ( !strcasecmp ( page, "Create" ) ) {
        /* create page and redirect */
        if ( ( wikitext = http_request_param_get ( req, "title" ) ) != NULL ) {
            wiki_redirect ( res, http_request_param_get ( req, "title" ) );
        } else {
            /* show create page form  */ wiki_show_create_page ( res );
        }
    } else if ( !strcasecmp ( page, "Replace" ) ) { /* Replace */
        if ( ( from = http_request_param_get ( req, "from" ) ) != NULL ) {
            if ( ( to = http_request_param_get ( req, "to" ) ) != NULL ) {
                wiki_replace ( res, from, to );
                wiki_show_tools_page ( res ); /* and again  */
            } else {
                /* show tools page form  */ wiki_show_tools_page ( res );
            }
        } else {
            /* show tools page form  */ wiki_show_tools_page ( res );
        }
    } else if ( !strcasecmp ( page, "Tools" ) ) { /* Show Tools */
        wiki_show_tools_page ( res );
        /* Replace */
        /* ... */
    } else {
        if ( ( wikitext = http_request_param_get ( req, "wikitext" ) ) != NULL ) {
            /* TODO: dont blindly write wikitext data to disk */
            /* http_request_param_showall(req,res); */
            if ( ( name = http_request_param_get ( req, "Save" ) ) != NULL ) {
                chdir ( datadir );
                if ( strlen ( wikitext ) <= 65535 ) {
                    file_write ( page, wikitext );
                }
            } else if ( ( name = http_request_param_get ( req, "Saveinclude" ) ) != NULL ) {
                chdir ( datadir );
                if ( strlen ( wikitext ) <= 65535 ) {
                    file_write ( page, wikitext );
                }
                file_copy ( datadir,page,includedir,page );
                chdir ( datadir );
                remove ( page );
                wiki_show_create_page ( res );
            } else if ( ( name = http_request_param_get ( req, "Savesecret" ) ) != NULL ) {
                chdir ( datadir );
                if ( strlen ( wikitext ) <= 65535 ) {
                    file_write ( page, wikitext );
                }
                file_copy ( datadir,page,secretdir,page );
                chdir ( datadir );
                remove ( page );
                wiki_show_create_page ( res );
            }
        }
        /* Show page with action */
        if ( access ( page, R_OK ) == 0 ) {	/* page exists */
            if ( !strcasecmp ( command, "Edit" ) ) { /* print edit page */
                chdir ( datadir );
                wikitext = file_read ( page );
                wiki_show_edit_page ( res, wikitext, page );
            } else if ( !strcasecmp ( command, "Trash" ) ) { /* to trash */
                file_copy ( datadir,page,trashdir,page );
                chdir ( datadir );
                remove ( page );
                wiki_show_search_results_page ( res, "*" );
            } else if ( !strcasecmp ( command, "Include" ) ) { /* to include */
                file_copy ( datadir,page,includedir,page );
                chdir ( datadir );
                remove ( page );
                wiki_show_search_results_page ( res, "*" );
            } else if ( !strcasecmp ( command, "Secret" ) ) { /* to secret */
                file_copy ( datadir,page,secretdir,page );
                chdir ( datadir );
                remove ( page );
                wiki_show_search_results_page ( res, "*" );
            } else { /* show page */
                wikitext = file_read ( page );
                wiki_show_page ( res, wikitext, page );
            }
        } else {	/* page do not exists */
            if ( !strcasecmp ( command, "Create" ) || ( name != NULL ) ) {
                wiki_show_edit_page ( res, NULL, page );
            } else if ( !strcasecmp ( command, "Editinclude" ) ) { /* print edit page */
                chdir ( includedir );
                wikitext = file_read ( page );
                wiki_show_edit_page ( res, wikitext, page );
            } else if ( !strcasecmp ( command, "Editsecret" ) ) { /* print edit page */
                chdir ( secretdir );
                wikitext = file_read ( page );
                wiki_show_edit_page ( res, wikitext, page );
            } else if ( !strcasecmp ( command, "Freeinclude" ) ) { /* to data */
                file_copy ( includedir,page,datadir,page );
                chdir ( includedir );
                remove ( page );
                chdir ( datadir );
                wikitext = file_read ( page );
                wiki_show_page ( res, wikitext, page );
            } else if ( !strcasecmp ( command, "Freesecret" ) ) { /* to data */
                file_copy ( secretdir,page,datadir,page );
                chdir ( secretdir );
                remove ( page );
                chdir ( datadir );
                wikitext = file_read ( page );
                wiki_show_page ( res, wikitext, page );
            } else if ( !strcasecmp ( command, "Trashincludes" ) ) { /* move to trash */
                file_copy ( includedir,page,trashdir,page );
                chdir ( includedir );
                remove ( page );
                wiki_show_search_results_page ( res, "includes/*" );
            } else if ( !strcasecmp ( command, "Trashimages" ) ) { /* move to trash */
                file_copy ( imagedir,page,trashdir,page );
                chdir ( imagedir );
                remove ( page );
                wiki_show_search_results_page ( res, "images/*" );
            } else if ( !strcasecmp ( command, "Deletecategories" ) ) { /* erase */
                chdir ( categorydir );
                remove ( page );
                wiki_show_search_results_page ( res, "categories/*" );
            } else if ( !strcasecmp ( command, "Deletetrash" ) ) { /* erase */
                chdir ( trashdir );
                remove ( page );
                wiki_show_search_results_page ( res, "trash/*" );
            } else if ( !strcasecmp ( command, "Undelete" ) ) { /* remove from trash */
                int len = strlen ( page );
                if (	!strncasecmp ( page+len-4, ".gif", 4 ) || !strncasecmp ( page+len-4, ".png", 4 )  ||
                        !strncasecmp ( page+len-4, ".jpg", 4 ) || !strncasecmp ( page+len-5, ".jpeg", 5 ) ||
                        !strncasecmp ( page+len-4, ".ico", 4 ) || !strncasecmp ( page+len-4, ".tif", 4 )  ||
                        !strncasecmp ( page+len-5, ".tiff", 5 ) ) {
                    file_copy ( trashdir,page,imagedir,page );  /* Images */
                } else {
                    file_copy ( trashdir,page,datadir,page );  /* Pages */
                }
                chdir ( trashdir );
                remove ( page );
                wiki_show_search_results_page ( res, "trash/*" );
            } else { /*  Create Page, if it not exists */
                char buf[1024];
                snprintf ( buf, 1024, "%s?Create", page );
                wiki_redirect ( res, buf );
            }
        }
    }
}

/* ******************************************************************************** */

int
wiki_init ( char *didiwiki_home )
{
    struct stat st;

    if ( didiwiki_home ) {
        snprintf ( datadir, 512, "%s/%s", getenv ( "HOME" ), didiwiki_home );
    } else {
        if ( getenv ( "DIDIWIKIHOME" ) ) {
            snprintf ( datadir, 512, "%s", getenv ( "DIDIWIKIHOME" ) );
        } else {
            if ( getenv ( "HOME" ) == NULL ) {
                fprintf ( stderr, "Unable to get home directory, is HOME set?\n" );
                exit ( 1 );
            }
            snprintf ( datadir, 512, "%s/.didiwiki", getenv ( "HOME" ) );
        }
    }
    snprintf ( imagedir, 524, "%s/images", datadir );
    snprintf ( includedir, 524, "%s/includes", datadir );
    snprintf ( categorydir, 524, "%s/categories", datadir );
    snprintf ( secretdir, 524, "%s/secrets", datadir );
    snprintf ( trashdir, 524, "%s/trash", datadir );
    /* Check if ~/.didiwiki exists and create if not */
    /* Check if ~/.didiwiki/images exists and create if not */
    /* Check if ~/.didiwiki/includes exists and create if not */
    /* Check if ~/.didiwiki/category exists and create if not */
    if ( stat ( datadir, &st ) != 0 ) {
        if ( my_mkdir ( datadir, 0755 ) == -1 ) {
            fprintf ( stderr, "Unable to create '%s', giving up.\n", datadir );
            exit ( 1 );
        }
    }
    if ( stat ( imagedir, &st ) != 0 ) {
        if ( my_mkdir ( imagedir, 0755 ) == -1 ) {
            fprintf ( stderr, "Unable to create '%s', giving up.\n", imagedir );
            exit ( 1 );
        }
    }
    if ( stat ( includedir, &st ) != 0 ) {
        if ( my_mkdir ( includedir, 0755 ) == -1 ) {
            fprintf ( stderr, "Unable to create '%s', giving up.\n", includedir );
            exit ( 1 );
        }
    }
    if ( stat ( categorydir, &st ) != 0 ) {
        if ( my_mkdir ( categorydir, 0755 ) == -1 ) {
            fprintf ( stderr, "Unable to create '%s', giving up.\n", categorydir );
            exit ( 1 );
        }
    }
    if ( stat ( secretdir, &st ) != 0 ) {
        if ( my_mkdir ( secretdir, 0755 ) == -1 ) {
            fprintf ( stderr, "Unable to create '%s', giving up.\n", secretdir );
            exit ( 1 );
        }
    }
    if ( stat ( trashdir, &st ) != 0 ) {
        if ( my_mkdir ( trashdir, 0755 ) == -1 ) {
            fprintf ( stderr, "Unable to create '%s', giving up.\n", trashdir );
            exit ( 1 );
        }
    }
    chdir ( datadir );

    /* Write Default pages if it doesn't exist */
    if ( access ( "WikiHelp", R_OK ) != 0 ) file_write ( "WikiHelp", HELPTEXT );
    if ( access ( "WikiHome", R_OK ) != 0 ) file_write ( "WikiHome", HOMETEXT );

    /* Read in optional CSS data */
    if ( access ( "styles.css", R_OK ) == 0 ) CssData = file_read ( "styles.css" );

    return 1;
}
