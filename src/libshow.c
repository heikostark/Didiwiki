/* ******************************************************************************** */

void
/* Show wiki page */
wiki_show_page ( HttpResponse *res, char *wikitext, char *page )
{
    char *html_clean_wikitext = NULL;
    globalsub = 1;

    http_response_printf_alloc_buffer ( res, strlen ( wikitext ) *4 );

    wiki_show_header ( res, page, "", TRUE, FALSE );
    html_clean_wikitext = util_htmlize ( wikitext, strlen ( wikitext ) );
    wiki_print_data_as_html ( res, html_clean_wikitext );
    wiki_show_footer ( res );
    http_response_send ( res );
    exit ( 0 );
}

/* Show wiki subpage via include */
void
wiki_show_subpage ( HttpResponse *res, char *page )
{
    if ( globalsub < 64536 ) {
        char *wikitext = "";
        wikitext = file_read ( page );
        char *html_clean_wikitext = NULL;
        html_clean_wikitext = util_htmlize ( wikitext, strlen ( wikitext ) );
        wiki_print_data_as_html ( res, html_clean_wikitext );
        globalsub++;
    } else {
        http_response_printf ( res, "<b>!! Too many Subpages (> 65535) !!</b>" ) ;
    }
}

/* ******************************************************************************** */

void
wiki_show_edit_page ( HttpResponse *res, char *wikitext, char *page )
{
    wiki_show_header ( res, "Edit:", page, FALSE, FALSE );
    if ( wikitext == NULL ) wikitext = "";
    http_response_printf ( res, EDITFORM, page, wikitext );
    wiki_show_footer ( res );
    http_response_send ( res );
    exit ( 0 );
}

/* ******************************************************************************** */

void
wiki_show_create_page ( HttpResponse *res )
{
    wiki_show_header ( res, "Create New Page", "", FALSE, FALSE );
    http_response_printf ( res, CREATEFORM );
    wiki_show_footer ( res );
    http_response_send ( res );
    exit ( 0 );
}

void
wiki_show_tools_page ( HttpResponse *res )
{
    wiki_show_header ( res, "Tools", "", FALSE, FALSE );
    http_response_printf ( res, TOOLSFORM );
    wiki_show_footer ( res );
    http_response_send ( res );
    exit ( 0 );
}

/* ******************************************************************************** */

void
/* Show secret page */
wiki_show_secret_page ( HttpResponse *res, char *wikitext, char *page )
{
    char *html_clean_wikitext = NULL;
    globalsub = 1;

    http_response_printf_alloc_buffer ( res, strlen ( wikitext ) *4 );

    wiki_show_header ( res, "Secret:", page, TRUE, TRUE );
    html_clean_wikitext = util_htmlize ( wikitext, strlen ( wikitext ) );
    wiki_print_data_as_html ( res, html_clean_wikitext );
    wiki_show_footer ( res );
    http_response_send ( res );
    exit ( 0 );
}

void
wiki_show_login_page ( HttpResponse *res )
{
    wiki_show_header ( res, "Secret", "", FALSE, FALSE );
    http_response_printf ( res, LOGINFORM );
    wiki_show_footer ( res );
    http_response_send ( res );
    exit ( 0 );
}

/* ******************************************************************************** */

void
wiki_show_changes_page ( HttpResponse *res )
{
    WikiPageList **pages = NULL;
    int            n_pages, i;

    wiki_show_header ( res, "Changes", "", FALSE, FALSE );

    http_response_printf ( res, "<table width='100%' border='0'><tr>\n" );
    http_response_printf ( res, "   <td align='center' width='12.5%'>&nbsp;</td>\n" );
    http_response_printf ( res, "   <td align='center' width='12.5%'>[<a href='/Changes'>Changes</a>]</td>\n" );
    http_response_printf ( res, "   <td align='center' width='12.5%'>[<a href='/Search?expr=*'>Data</a>]</td>\n" );
    http_response_printf ( res, "   <td align='center' width='12.5%'>[<a href='/Search?expr=includes/*'>Includes</a>]</td>\n" );
    http_response_printf ( res, "   <td align='center' width='12.5%'>[<a href='/Search?expr=images/*'>Images</a>]</td>\n" );
    http_response_printf ( res, "   <td align='center' width='12.5%'>[<a href='/Search?expr=categories/*'>Categories</a>]</td>\n" );
    http_response_printf ( res, "   <td align='center' width='12.5%'>[<a href='/Search?expr=trash/*'>Trash</a>]</td>\n" );
    http_response_printf ( res, "   <td align='center' width='12.5%'>&nbsp;</td>\n" );
    http_response_printf ( res, "</tr></table>\n" );
    http_response_printf ( res, "<br /><hr /><br />\n" );

    pages = wiki_get_pages ( &n_pages, datadir, NULL ); /* only pages */
    for ( i=0; i<n_pages; i++ ) {
        struct tm   *pTm;
        char   datebuf[64];

        pTm = localtime ( &pages[i]->mtime );
        strftime ( datebuf, sizeof ( datebuf ), "%Y-%m-%d %H:%M", pTm );
        http_response_printf ( res, "%s --- [<a href='%s?Trash'>Trash</a>] --- [<a href='%s?Secret'>Secret</a>] | [<a href='%s?Include'>Include</a>] | [<a href='%s?Edit'>Edit</a>] --- <a href='%s'>%s</a><br />\n", datebuf, pages[i]->name, pages[i]->name, pages[i]->name, pages[i]->name, pages[i]->name, pages[i]->name );
        if ( ( i % 10 ) == 9 ) {
            http_response_printf ( res, "<br />\n" );
        }
    }
    wiki_show_footer ( res );
    http_response_send ( res );
    exit ( 0 );
}

/* ******************************************************************************** */

void
wiki_show_search_results_page ( HttpResponse *res, char *expr )
{
    WikiPageList **pages = NULL;
    int            n_pages, i;
    int            star = 0;
    char           *newexpr = "";

	if (expr == NULL || strlen(expr) == 0) 
	{ 
		expr = malloc(sizeof(char) * 2); 
		memset(expr, 0, sizeof(char) * 2 ); 		
		newexpr = malloc(sizeof(char) * 2); 
		memset(newexpr, 0, sizeof(char) * 2 ); 
	}
	else 
	{ 
		newexpr = malloc(sizeof(char) * (strlen(expr)+1) ); 
		memset(newexpr, 0, sizeof(char) * (strlen(expr)+1) ); 
	}

    if ( expr == NULL || strlen ( expr ) == 0 ) {
        star = -1;
    } else if ( *expr == '*' ) {
        newexpr[0] = '*';
        star = 1;
        pages = wiki_get_searchpages ( &n_pages, datadir, newexpr );
    } else if ( !strncasecmp ( expr, "categories/*", 12 ) ) {
        newexpr[0] = '*';
        star = 2;
        chdir ( categorydir );
        pages = wiki_get_searchpages ( &n_pages, categorydir, newexpr );
    } else if ( !strncasecmp ( expr, "includes/*", 10 ) ) {
        newexpr[0] = '*';
        star = 3;
        chdir ( includedir );
        pages = wiki_get_searchpages ( &n_pages, includedir, newexpr );
    } else if ( !strncasecmp ( expr, "images/*", 8 ) ) {
        newexpr[0] = '*';
        star = 4;
        chdir ( imagedir );
        pages = wiki_get_searchpages ( &n_pages, imagedir, newexpr );
    } else if ( !strncasecmp ( expr, "trash/*", 8 ) ) {
        newexpr[0] = '*';
        star = 5;
        chdir ( trashdir );
        pages = wiki_get_searchpages ( &n_pages, trashdir, newexpr );
    } else {
        strcpy ( newexpr, expr );
        chdir ( datadir );
        pages = wiki_get_searchpages ( &n_pages, datadir, newexpr );
    }

    wiki_show_header ( res, "Search:",expr, FALSE, FALSE );

    http_response_printf ( res, "<table width='100%' border='0'><tr>\n" );
    http_response_printf ( res, "   <td align='center' width='12.5%'>&nbsp;</td>\n" );
    http_response_printf ( res, "   <td align='center' width='12.5%'>[<a href='/Changes'>Changes</a>]</td>\n" );
    http_response_printf ( res, "   <td align='center' width='12.5%'>[<a href='/Search?expr=*'>Data</a>]</td>\n" );
    http_response_printf ( res, "   <td align='center' width='12.5%'>[<a href='/Search?expr=includes/*'>Includes</a>]</td>\n" );
    http_response_printf ( res, "   <td align='center' width='12.5%'>[<a href='/Search?expr=images/*'>Images</a>]</td>\n" );
    http_response_printf ( res, "   <td align='center' width='12.5%'>[<a href='/Search?expr=categories/*'>Categories</a>]</td>\n" );
    http_response_printf ( res, "   <td align='center' width='12.5%'>[<a href='/Search?expr=trash/*'>Trash</a>]</td>\n" );
    http_response_printf ( res, "   <td align='center' width='12.5%'>&nbsp;</td>\n" );
    http_response_printf ( res, "</tr></table>\n" );
    http_response_printf ( res, "<br /><hr /><br />\n" );

    if ( ( pages ) && ( *newexpr != '*' ) ) {
        /* for pages */  /* redirect on page name match */
        for ( i=0; i<n_pages; i++ )
            if ( !strcasecmp ( pages[i]->name, newexpr ) ) wiki_redirect ( res, pages[i]->name );
        /* for text */
        for ( i=n_pages-1; i>-1; i-- ) {
            http_response_printf ( res, "[<a href='%s?Trash'>Trash</a>] --- [<a href='%s?Secret'>Secret</a>] | [<a href='%s?Include'>Include</a>] | [<a href='%s?Edit'>Edit</a>] --- <a href='%s'>%s</a><br />\n", pages[i]->name, pages[i]->name, pages[i]->name, pages[i]->name, pages[i]->name, pages[i]->name );
            if ( ( ( n_pages-1-i ) % 10 ) == 9 ) {
                http_response_printf ( res, "<br />\n" );
            }
        }
    } else if ( ( pages ) && ( *newexpr == '*' ) ) {
        if ( star == 1 ) { /* star */
            for ( i=n_pages-1; i>-1; i-- ) {
                http_response_printf ( res, "[<a href='%s?Trash'>Trash</a>] --- [<a href='%s?Secret'>Secret</a>] | [<a href='%s?Include'>Include</a>] | [<a href='%s?Edit'>Edit</a>] --- <a href='%s'>%s</a><br />\n", pages[i]->name, pages[i]->name, pages[i]->name, pages[i]->name, pages[i]->name, pages[i]->name );
                if ( ( ( n_pages-1-i ) % 10 ) == 9 ) {
                    http_response_printf ( res, "<br />\n" );
                }
            }
        } else if ( star == 2 ) { /* categories */
            for ( i=n_pages-1; i>-1; i-- ) {
                http_response_printf ( res, "[<a href='%s?Deletecategories'>Delete</a>] --- <a href='Category?expr=[Category:%s'>%s</a><br />\n", pages[i]->name, pages[i]->name, pages[i]->name );
                if ( ( ( n_pages-1-i ) % 10 ) == 9 ) {
                    http_response_printf ( res, "<br />\n" );
                }
            }
        } else if ( star == 3 ) { /* includes */
            for ( i=n_pages-1; i>-1; i-- ) {
                http_response_printf ( res, "[<a href='%s?Trashincludes'>Trash</a>] --- [<a href='%s?Freeinclude'>Free</a>] | [<a href='%s?Editinclude'>Edit</a>] --- %s<br />\n", pages[i]->name, pages[i]->name, pages[i]->name, pages[i]->name );
                if ( ( ( n_pages-1-i ) % 10 ) == 9 ) {
                    http_response_printf ( res, "<br />\n" );
                }
            }
        } else if ( star == 4 ) { /* images */
            for ( i=n_pages-1; i>-1; i-- ) {
                http_response_printf ( res, "[<a href='%s?Trashimages'>Trash</a>] --- <div class='tooltip'><a href='images/%s'>%s</a><img src='images/%s' class='tooltiptext'></div><br />\n", pages[i]->name, pages[i]->name, pages[i]->name, pages[i]->name );
                if ( ( ( n_pages-1-i ) % 10 ) == 9 ) {
                    http_response_printf ( res, "<br />\n" );
                }
            }
        }
        /* https://stackoverflow.com/questions/10769016/display-image-on-text-link-hover-css-only */
        else if ( star == 5 ) { /* images */
            for ( i=n_pages-1; i>-1; i-- ) {
                http_response_printf ( res, "[<a href='%s?Deletetrash'>Delete</a>] --- [<a href='%s?Undelete'>Undelete</a>] --- %s<br />\n", pages[i]->name, pages[i]->name, pages[i]->name );
                if ( ( ( n_pages-1-i ) % 10 ) == 9 ) {
                    http_response_printf ( res, "<br />\n" );
                }
            }
        } else {
            for ( i=n_pages-1; i>-1; i-- ) {
                http_response_printf ( res, "%s<br />\n", pages[i]->name );
            }
        }
    } else  if ( star == -1 )
    { } /* for empty pages */
    else {
        http_response_printf ( res, "No search matches" );
    }
    wiki_show_footer ( res );
    http_response_send ( res );
    exit ( 0 );
}

void
wiki_show_find_results_page ( HttpResponse *res, char *expr )
{
    WikiPageList **pages = NULL;
    int            n_pages, i;

    wiki_show_header ( res, "Find:", expr, FALSE, FALSE );
    if ( expr == NULL || strlen ( expr ) == 0 ) {
        http_response_printf ( res, "No Find Terms supplied" );
    } else {
        pages = wiki_get_pages ( &n_pages, datadir, expr ); /* only pages */
        if ( pages ) {
            for ( i=n_pages-1; i>-1; i-- ) {
                http_response_printf ( res, "<a href='%s'>%s</a><br />\n", pages[i]->name, pages[i]->name );
                if ( ( ( n_pages-1-i ) % 10 ) == 9 ) {
                    http_response_printf ( res, "<br />\n" );
                }
            }
        } else  {
            http_response_printf ( res, "No find matches" );
        }
    }
    wiki_show_footer ( res );
    http_response_send ( res );
    exit ( 0 );
}

void
wiki_show_included_search_results_page ( HttpResponse *res, char *url, char *expr ) /*Include Search */
{
    WikiPageList **pages = NULL;
    int            n_pages, i;

    if ( expr == NULL || strlen ( expr ) == 0 ) {
        http_response_printf ( res, "No Search Terms supplied" );
    }
    pages = wiki_get_searchpages ( &n_pages, url, expr );
    if ( pages ) {
        for ( i=0; i<n_pages; i++ )
            if ( !strcasecmp ( pages[i]->name, expr ) ) /* redirect on page name match */ wiki_redirect ( res, pages[i]->name );
        for ( i=n_pages-1; i>-1; i-- ) {
            http_response_printf ( res, "<a href='%s'>%s</a><br />\n", pages[i]->name, pages[i]->name );
            if ( ( ( n_pages-1-i ) % 10 ) == 9 ) {
                http_response_printf ( res, "<br />\n" );
            }
        }
    } else {
        http_response_printf ( res, "No search matches" );
    }
}

/* ******************************************************************************** */

void
wiki_show_category_results_page ( HttpResponse *res, char *expr )
{
    WikiPageList **pages = NULL;
    int            n_pages, i;
    char           *newexpr = "";

    newexpr = malloc ( sizeof ( char ) * ( strlen ( expr )-10 + 1 ) );
    memset ( newexpr, 0, sizeof ( char ) * ( strlen ( expr )-10 + 1 ) );
    strncpy ( newexpr, expr+10, strlen ( expr )-10 );

    wiki_show_header ( res, "Category:", newexpr, FALSE, FALSE );
    if ( expr == NULL || strlen ( expr ) == 0 ) {
        http_response_printf ( res, "No Category Terms supplied" );
    } 
    else 
    {
        pages = wiki_get_searchpages ( &n_pages, datadir, expr );
        if ( pages ) 
        {
            for ( i=0; i<n_pages; i++ )
                if ( !strcasecmp ( pages[i]->name, expr ) ) /* redirect on page name match */ wiki_redirect ( res, pages[i]->name );
            for ( i=n_pages-1; i>-1; i-- ) 
            {
                http_response_printf ( res, "<a href='%s'>%s</a><br />\n", pages[i]->name, pages[i]->name );
                if ( ( ( n_pages-1-i ) % 10 ) == 9 ) { http_response_printf ( res, "<br />\n" ); }
            }
        } 
        else 
        {
            http_response_printf ( res, "No category matches" );
        }
    }
    wiki_show_footer ( res );
    http_response_send ( res );
    exit ( 0 );
}

void
wiki_show_included_category_results_page ( HttpResponse *res, char *expr ) /* Include Category */
{
    WikiPageList **pages = NULL;
    int            n_pages, i;
    char          *newexpr = "";

    if ( expr == NULL || strlen ( expr ) == 0 ) {
        http_response_printf ( res, "No Category Terms supplied" );
    }

    newexpr = malloc ( sizeof ( char ) * ( strlen ( expr )+10 + 1 ) );
    memset ( newexpr, 0, sizeof ( char ) * ( strlen ( expr )+10 + 1 ) );
    strcpy ( newexpr, "[Category:" );
    strcat ( newexpr,expr );
    pages = wiki_get_searchpages ( &n_pages, datadir, newexpr );
    if ( pages ) {
        for ( i=0; i<n_pages; i++ )
            if ( !strcasecmp ( pages[i]->name, newexpr ) ) /* redirect on page name match */ wiki_redirect ( res, pages[i]->name );
        for ( i=n_pages-1; i>-1; i-- ) {
            http_response_printf ( res, "<a href='%s'>%s</a><br />\n", pages[i]->name, pages[i]->name );
            if ( ( ( n_pages-1-i ) % 10 ) == 9 ) {
                http_response_printf ( res, "<br />\n" );
            }
        }
    } else {
        http_response_printf ( res, "No category matches!" );
    }
}

/* ******************************************************************************** */

void
wiki_replace ( HttpResponse *res, char *from, char *to )
{
    WikiPageList **pages = NULL;
    int          n_pages, i;
    static char  syntax[512] = "\0";
    char         *wikitext = "";

    pages = wiki_get_pages ( &n_pages, datadir, "*" ); /* only pages */
    if ( pages ) {
        for ( i=n_pages-1; i>-1; i-- ) {
            if ( access ( pages[i]->name, R_OK ) == 0 ) {	/* page exists */
                wikitext = file_read ( pages[i]->name );
                if ( strstr ( wikitext,from ) != NULL ) {
                    sprintf ( syntax, "sed -e ':a;s/%s/{sed-replaced}/;ta;:b;s/{sed-replaced}/%s/;tb' -i '%s'", from, to, pages[i]->name );
                    system ( syntax );
                    http_response_printf ( res, "Text in (%s) replaced!<br />\n", pages[i]->name );
                }
            } else {
                http_response_printf ( res, "No write access to (%s)!<br />\n", pages[i]->name );
            }
        }
        http_response_printf ( res, "<br />\nDone.<br />\n<br />\n" );
    }
}

/* ******************************************************************************** */

void
wiki_show_template ( HttpResponse *res, char *template_data )
{
    /* 4 templates - header.html, footer.html,
                     header-noedit.html, footer-noedit.html
       Vars;
       $title      - page title.
       $include()  - ?
       $pages
    */
}

void
wiki_show_header ( HttpResponse *res, char *page_title, char *page_title2, int want_edit, int want_login )
{
    static char page_short[512] = "\0";
    http_response_printf ( res,
                           "<!DOCTYPE html>\n"
                           "<head>\n"
                           "<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />\n"
                           "<link rel='SHORTCUT ICON' href='/favicon.ico' />\n"
                           "<link media='all' href='/styles.css' rel='stylesheet' type='text/css' />\n"
                           "<title>%s%s</title>\n"
                           "</head>\n"
                           "<body>\n",
                           page_title,
                           page_title2
                         );
    if ( want_login ) {
        /* for too long page names */
        int len = strlen ( page_title2 );
        if ( len >= 40 ) {
            strncpy ( page_short, page_title2, 40 );
            strncpy ( page_short+36, "...\0", 4 );
        } else strcpy ( page_short, page_title2 );

        http_response_printf ( res,
                               PAGEHEADER,
                               page_title,
                               page_short,
                               ( want_edit ) ? " ( <a href='/Find?expr=" : "",
                               ( want_edit ) ? page_title2 : "",
                               ( want_edit ) ? "' title='Find this wiki page in contents. [alt-f]' accesskey='f'>Find</a> |" : "",
                               ( want_edit ) ? "<a href='http://www.google.de/search?hl=de&q=\"" : "",
                               ( want_edit ) ? page_title2 : "",
                               ( want_edit ) ? "\"' title='Search this wiki page in google. [alt-g]' accesskey='g'>Google</a> |" : "",
                               ( want_edit ) ? "<a href='" : "",
                               ( want_edit ) ? page_title2 : "",
                               ( want_edit ) ? "?Freesecret' title='Free this wiki page contents.'>Free</a> |" : "",
                               ( want_edit ) ? "<a href='" : "",
                               ( want_edit ) ? page_title2 : "",
                               ( want_edit ) ? "?Editsecret' title='Edit this wiki page contents.'>Edit</a>)" : "" );
    } else {
        /* for too long page names */
        int len = strlen ( page_title );
        if ( len >= 40 ) {
            strncpy ( page_short, page_title, 40 );
            strncpy ( page_short+36, "...\0", 4 );
        } else strcpy ( page_short, page_title );

        http_response_printf ( res,
                               PAGEHEADER,
                               page_short,
                               page_title2,
                               ( want_edit ) ? " ( <a href='/Find?expr=" : "",
                               ( want_edit ) ? page_title : "",
                               ( want_edit ) ? "' title='Find this wiki page in contents. [alt-f]' accesskey='f'>Find</a> |" : "",
                               ( want_edit ) ? "<a href='http://www.google.de/search?hl=de&q=\"" : "",
                               ( want_edit ) ? page_title : "",
                               ( want_edit ) ? "\"' title='Search this wiki page in google. [alt-g]' accesskey='g'>Google</a> |" : "",
                               ( want_edit ) ? "<a href='" : "",
                               ( want_edit ) ? page_title : "",
                               ( want_edit ) ? "?Edit' title='Edit this wiki page contents. [alt-e]' accesskey='e'>Edit</a> )" : "",
                               "", "", "" );

    }
}

void
wiki_show_footer ( HttpResponse *res )
{
    http_response_printf ( res, "%s", PAGEFOOTER );
    http_response_printf ( res, "</body>\n</html>\n" );
}
