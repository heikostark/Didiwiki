static int
is_wiki_format_char_or_space ( char c )
{
    if ( isspace ( c ) ) return 1;
    if ( strchr ( "/*^_-=", c ) ) return 1;
    return 0;
}

static char*
get_line_from_string ( char **lines, int *line_len )
{
    int   i;
    char *z = *lines;
    if ( z[0] == '\0' ) return NULL;
    for ( i=0; z[i]; i++ ) {
        if ( z[i] == '\n' ) {
            if ( i > 0 && z[i-1]=='\r' ) {
                z[i-1] = '\0';
            } else {
                z[i] = '\0';
            }
            i++;
            break;
        }
    }
    /* advance lines on */
    *lines      = &z[i];
    *line_len -= i;
    return z;
}

static char*
check_for_link ( char *line, int *skip_chars )
{
    char *start  =  line;
    char *p      =  line;
    char *url    =  NULL;
    char *title  =  NULL;
    char *title2 =  NULL;
    char *result = NULL;
    int   found  = 0;
    
    /* [ link ] | [ link | title ] | [ link | title | title2 ] */
    
    if ( *p == '[' ) 
    {
        url = start+1;
        *p = '\0';
        p++;
        if (	!strncasecmp ( p, "http://", 7 ) || !strncasecmp ( p, "https://", 8 ) || !strncasecmp ( p, "mailto://", 9 ) ||
                !strncasecmp ( p, "file://", 7 ) || !strncasecmp ( p, "ftp://", 6 ) ||!strncasecmp ( p, "ftps://", 7 ) ||
                !strncasecmp ( p, "category:", 9 ) ||
                !strncasecmp ( p, "frame:", 6 ) ||
                !strncasecmp ( p, "image:", 6 ) || !strncasecmp ( p, "image100:", 9 ) || !strncasecmp ( p, "image50:", 8 ) ||
                !strncasecmp ( p, "image66:", 6 ) || !strncasecmp ( p, "image33:", 8 ) || !strncasecmp ( p, "image25:", 8 ) ||
                !strncasecmp ( p, "smallicon:", 10 ) || !strncasecmp ( p, "icon:", 5 ) ||  !strncasecmp ( p, "bigicon:", 8 ) ) {
            /* http, mailto, file and category has no included space chars */
            while ( *p != ']' && *p != '\0' && *p != '|' && !isspace ( *p ) ) p++;
        } else {
            while ( *p != ']' && *p != '\0' && *p != '|' ) p++;
        }

        if ( *p == '|' ) {
            *p = '\0';
            p++;
            title = p;
            while ( *p != ']' && *p != '\0' && *p != '|' ) p++;
            if ( *p == '|' ) {
                *p = '\0';
                p++;
                title2 = p;
                while ( *p != ']' && *p != '\0' && *p != '|' ) p++;
                *p = '\0';
                p++;
            } else {
                *p = '\0';
                p++;
            }
        } else {
            *p = '\0';
            p++;
        }
    } 
    else if (	!strncasecmp ( p, "http://", 7 ) || !strncasecmp ( p, "https://", 8 ) || !strncasecmp ( p, "mailto://", 9 ) ||
                !strncasecmp ( p, "file://", 7 ) || !strncasecmp ( p, "ftp://", 6 ) ||!strncasecmp ( p, "ftps://", 7 ) ) 
    {
        while ( *p != '\0' && !isspace ( *p ) ) p++;
        found = 1;
    }

    if ( found ) 
    { /* cant really set http/camel links in place */
        url = malloc( sizeof ( char ) * ( ( p - start ) + 2 ) );
        memset ( url, 0, sizeof ( char ) * ( ( p - start ) + 2 ) );
        strncpy ( url, start, p - start );
        *start = '\0';
    }

    /*if (strcasecmp (oldpage,url))
    {*/
    if ( url != NULL ) 
    {
        int len = strlen ( url );
        *skip_chars = p - start;
        /* is it an image ? */
        if (	!strncasecmp ( url+len-4, ".gif", 4 ) || !strncasecmp ( url+len-4, ".png", 4 )  ||
                !strncasecmp ( url+len-4, ".jpg", 4 ) || !strncasecmp ( url+len-5, ".jpeg", 5 ) ||
                !strncasecmp ( url+len-4, ".ico", 4 ) || !strncasecmp ( url+len-4, ".tif", 4 )  ||
                !strncasecmp ( url+len-5, ".tiff", 5 )  || !strncasecmp ( url+len-6, "mapnik", 6 ) ) /* TODO mapnik for frame? */ 
        {
            if ( !strncasecmp ( url, "http://", 7 ) || !strncasecmp ( url, "https://", 8 ) ) 
            {
                if ( title2 ) asprintf ( &result, "<img style='%s' alt='%s' src='%s' border='0'>", title, title2, url );
                else if ( title ) asprintf ( &result, "<img alt='%s' src='%s' border='0'>", title, url );
                else asprintf ( &result, "<img src='%s' border='0'>", url );
            } 
            else 
            {
                char *image = "";
                image = malloc ( sizeof ( char ) * ( ( p - start ) + 2 ) );
                memset ( image, 0, sizeof ( char ) * ( ( p - start ) + 2 ) );
                
                if ( !strncasecmp ( url, "frame:", 6 ) ) 
                { /* regular images */
                    strncpy ( image, url+6, p-url );
                    asprintf ( &result, "<iframe width='425' height='350' frameborder='0' scrolling='no' marginheight='0' marginwidth='0' src='%s' style='border: 1px solid black'>", image );                    
                } 
                
                else if ( !strncasecmp ( url, "image:", 6 ) ) 
                { /* regular images */
                    strncpy ( image, url+6, p-url );
                    if ( !strncasecmp ( image, "http://", 7 ) || !strncasecmp ( image, "https://", 8 ) ) {
                        if ( title2 ) asprintf ( &result, "<img style='%s' alt='%s' src='%s' border='0'>", title, title2, image );
                        else if ( title ) asprintf ( &result, "<img alt='%s' src='%s' border='0'>", title, image );
                        else asprintf ( &result, "<img src='%s' border='0'>", image );
                    } else {
                        if ( title2 ) asprintf ( &result, "<img style='%s' alt='%s' src='/images/%s' border='0'>", title, title2, image );
                        else if ( title ) asprintf ( &result, "<img alt='%s' src='/images/%s' border='0'>", title, image );
                        else asprintf ( &result, "<img src='/images/%s' border='0'>", image );
                    }
                } 
                else if ( !strncasecmp ( url, "image100:", 9 ) ) 
                { /* regular images */
                    strncpy ( image, url+8, p-url );
                    if ( !strncasecmp ( image, "http://", 7 ) || !strncasecmp ( image, "https://", 8 ) ) {
                        if ( title2 ) asprintf ( &result, "<img style='width:100%%;%s' alt='%s' src='%s' border='0'>", title, title2, image );
                        else if ( title ) asprintf ( &result, "<img style='width:100%%' alt='%s' src='%s' border='0'>", title, image );
                        else asprintf ( &result, "<img style='width:100%%' src='%s' border='0'>", image );
                    } else {
                        if ( title2 ) asprintf ( &result, "<img style='width:100%%;%s' alt='%s' src='/images/%s' border='0'>", title, title2, image );
                        else if ( title ) asprintf ( &result, "<img style='width:100%%'alt='%s' src='/images/%s' border='0'>", title, image );
                        else asprintf ( &result, "<img style='width:100%%' src='/images/%s' border='0'>", image );
                    }
                } 
                else if ( !strncasecmp ( url, "image66:", 8 ) ) 
                { /* regular images */
                    strncpy ( image, url+8, p-url );
                    if ( !strncasecmp ( image, "http://", 7 ) || !strncasecmp ( image, "https://", 8 ) ) {
                        if ( title2 ) asprintf ( &result, "<img style='width:66%%;%s' alt='%s' src='%s' border='0'>", title, title2, image );
                        else if ( title ) asprintf ( &result, "<img style='width:66%%' alt='%s' src='%s' border='0'>", title, image );
                        else asprintf ( &result, "<img style='width:66%%' src='%s' border='0'>", image );
                    } else {
                        if ( title2 ) asprintf ( &result, "<img style='width:66%%;%s' alt='%s' src='/images/%s' border='0'>", title, title2, image );
                        else if ( title ) asprintf ( &result, "<img style='width:66%%'alt='%s' src='/images/%s' border='0'>", title, image );
                        else asprintf ( &result, "<img style='width:66%%' src='/images/%s' border='0'>", image );
                    }
                } 
                else if ( !strncasecmp ( url, "image50:", 8 ) ) 
                { /* regular images */
                    strncpy ( image, url+8, p-url );
                    if ( !strncasecmp ( image, "http://", 7 ) || !strncasecmp ( image, "https://", 8 ) ) {
                        if ( title2 ) asprintf ( &result, "<img style='width:50%%;%s' alt='%s' src='%s' border='0'>", title, title2, image );
                        else if ( title ) asprintf ( &result, "<img style='width:50%%' alt='%s' src='%s' border='0'>", title, image );
                        else asprintf ( &result, "<img style='width:50%%' src='%s' border='0'>", image );
                    } else {
                        if ( title2 ) asprintf ( &result, "<img style='width:50%%;%s' alt='%s' src='/images/%s' border='0'>", title, title2, image );
                        else if ( title ) asprintf ( &result, "<img style='width:50%%'alt='%s' src='/images/%s' border='0'>", title, image );
                        else asprintf ( &result, "<img style='width:50%%' src='/images/%s' border='0'>", image );
                    }
                } 
                else if ( !strncasecmp ( url, "image33:", 8 ) ) 
                { /* regular images */
                    strncpy ( image, url+8, p-url );
                    if ( !strncasecmp ( image, "http://", 7 ) || !strncasecmp ( image, "https://", 8 ) ) {
                        if ( title2 ) asprintf ( &result, "<img style='width:33%%;%s' alt='%s' src='%s' border='0'>", title, title2, image );
                        else if ( title ) asprintf ( &result, "<img style='width:33%%' alt='%s' src='%s' border='0'>", title, image );
                        else asprintf ( &result, "<img style='width:33%%' src='%s' border='0'>", image );
                    } else {
                        if ( title2 ) asprintf ( &result, "<img style='width:33%%;%s' alt='%s' src='/images/%s' border='0'>", title, title2, image );
                        else if ( title ) asprintf ( &result, "<img style='width:33%%'alt='%s' src='/images/%s' border='0'>", title, image );
                        else asprintf ( &result, "<img style='width:33%%' src='/images/%s' border='0'>", image );
                    }
                } 
                else if ( !strncasecmp ( url, "image25:", 8 ) ) 
                { /* regular images */
                    strncpy ( image, url+8, p-url );
                    if ( !strncasecmp ( image, "http://", 7 ) || !strncasecmp ( image, "https://", 8 ) ) {
                        if ( title2 ) asprintf ( &result, "<img style='width:25%%;%s' alt='%s' src='%s' border='0'>", title, title2, image );
                        else if ( title ) asprintf ( &result, "<img style='width:25%%' alt='%s' src='%s' border='0'>", title, image );
                        else asprintf ( &result, "<img style='width:25%%' src='%s' border='0'>", image );
                    } else {
                        if ( title2 ) asprintf ( &result, "<img style='width:25%%;%s' alt='%s' src='/images/%s' border='0'>", title, title2, image );
                        else if ( title ) asprintf ( &result, "<img style='width:25%%'alt='%s' src='/images/%s' border='0'>", title, image );
                        else asprintf ( &result, "<img style='width:25%%' src='/images/%s' border='0'>", image );
                    }
                } 
                else if ( !strncasecmp ( url, "smallicon:", 10 ) ) 
                {
                    strncpy ( image, url+10, p-url );
                    if ( !strncasecmp ( image, "http://", 7 ) || !strncasecmp ( image, "https://", 8 ) ) {
                        if ( title2 ) asprintf ( &result, "<img style='height:16px;%s' alt='%s' src='%s' border='0'>", title, title2, image );
                        else if ( title ) asprintf ( &result, "<img style='height:16px' alt='%s' src='%s' border='0'>", title, image );
                        else asprintf ( &result, "<img style='height:16px' src='%s' border='0'>", image );
                    } else {
                        if ( title2 ) asprintf ( &result, "<img style='height:16px;%s' alt='%s' src='/images/%s' border='0'>", title, title2, image );
                        else if ( title ) asprintf ( &result, "<img style='height:16px' alt='%s' src='/images/%s' border='0'>", title, image );
                        else asprintf ( &result, "<img style='height:16px' src='/images/%s' border='0'>", image );
                    }
                } 
                else if ( !strncasecmp ( url, "icon:", 5 ) ) 
                {
                    strncpy ( image, url+5, p-url );
                    if ( !strncasecmp ( image, "http://", 7 ) || !strncasecmp ( image, "https://", 8 ) ) {
                        if ( title2 ) asprintf ( &result, "<img style='height:32px;%s' alt='%s' src='%s' border='0'>", title, title2, image );
                        else if ( title ) asprintf ( &result, "<img style='height:32px' alt='%s' src='%s' border='0'>", title, image );
                        else asprintf ( &result, "<img style='height:32px' src='%s' border='0'>", image );
                    } else {
                        if ( title2 ) asprintf ( &result, "<img style='height:32px;%s' alt='%s' src='/images/%s' border='0'>", title, title2, image );
                        else if ( title ) asprintf ( &result, "<img style='height:32px' alt='%s' src='/images/%s' border='0'>", title, image );
                        else asprintf ( &result, "<img style='height:32px' src='/images/%s' border='0'>", image );
                    }
                } 
                else if ( !strncasecmp ( url, "bigicon:", 8 ) ) {
                    strncpy ( image, url+8, p-url );
                    if ( !strncasecmp ( image, "http://", 7 ) || !strncasecmp ( image, "https://", 8 ) ) {
                        if ( title2 ) asprintf ( &result, "<img style='height:64px;%s' alt='%s' src='%s' border='0'>", title, title2, image );
                        else if ( title ) asprintf ( &result, "<img style='height:64px' alt='%s' src='%s' border='0'>", title, image );
                        else asprintf ( &result, "<img style='height:64px' src='%s' border='0'>", image );
                    } else {
                        if ( title2 ) asprintf ( &result, "<img style='height:64px;%s' alt='%s' src='/images/%s' border='0'>", title, title2, image );
                        else if ( title ) asprintf ( &result, "<img style='height:64px' alt='%s' src='/images/%s' border='0'>", title, image );
                        else asprintf ( &result, "<img style='height:64px' src='/images/%s' border='0'>", image );
                    }
                } else {
                    strncpy ( image, url, p-url );
                    if ( title2 ) asprintf ( &result, "<img style='%s' alt='%s' src='%s' border='0'>", title, title2, image );
                    else if ( title ) asprintf ( &result, "<img alt='%s' src='%s' border='0'>", title, image );
                    else asprintf ( &result, "<img src='%s' border='0'>", image );
                }
            }
        }
        /* is it an category ? */
        else if ( !strncasecmp ( url, "category:", 9 ) ) {
            char *category = "";
            category = malloc ( sizeof ( char ) * ( ( p - start ) + 2 ) );
            memset ( category, 0, sizeof ( char ) * ( ( p - start ) + 2 ) );
            strncpy ( category, url+9, p-url );
            if ( title ) asprintf ( &result,"<a href='Category?expr=[Category:%s'><b>%s</b></a>", category, title );
            else asprintf ( &result, "<a href='Category?expr=[Category:%s'><b>%s</b></a>", category, category );
            chdir ( "categories" );
            file_write ( category,category );
            chdir ( ".." );
        } else if ( !strncasecmp ( url, ":category:", 10 ) ) {
            char *category = "";
            category = malloc ( sizeof ( char ) * ( ( p - start ) + 2 ) );
            memset ( category, 0, sizeof ( char ) * ( ( p - start ) + 2 ) );
            strncpy ( category, url+10, p-url );
            if ( title ) asprintf ( &result,"<a href='Category?expr=[Category:%s'><b>%s</b></a>", category, title );
            else asprintf ( &result, "<a href='Category?expr=[Category:%s'><b>%s</b></a>", category, category );
        }
        /* is it an link ? */
        else {
            if ( title ) asprintf ( &result,"<a href='%s'>%s</a>", url, title );
            else asprintf ( &result, "<a href='%s'>%s</a>", url, url );
        }
        return result;
    }
    return NULL;
}

static char*
check_for_html ( char *line, int *skip_chars )
{
    char *start =  line;
    char *p     =  line;
    char *url   =  NULL;
    char *result = NULL;
    int   found = 0;

    if ( !strncasecmp ( p, "&lt;html&gt;", 12 ) ) 
    {
        while ( *p != '\0' && !isspace ( *p ) && *p != ';' ) p++;
        if ( *p == ';' ) {
            p++;
        }
        while ( *p != '\0' && !isspace ( *p ) && *p != ';' ) p++;
        if ( *p == ';' ) {
            p++;
        }
        found = 1;
        html_found = 1;
    } 
    else if ( !strncasecmp ( p, "&lt;/html&gt;", 13 ) ) 
    {
        while ( *p != '\0' && !isspace ( *p ) && *p != ';' ) p++;
        if ( *p == ';' ) {
            p++;
        }
        while ( *p != '\0' && !isspace ( *p ) && *p != ';' ) p++;
        if ( *p == ';' ) {
            p++;
        }
        found = 1;
        html_found = 0;
    } 
    else if ( !strncasecmp ( p, "&lt;math&gt;", 12 ) ) 
    {
        while ( *p != '\0' && !isspace ( *p ) && *p != ';' ) p++;
        if ( *p == ';' ) {
            p++;
        }
        while ( *p != '\0' && !isspace ( *p ) && *p != ';' ) p++;
        if ( *p == ';' ) {
            p++;
        }
        found = 1;
        math_found = 1;
    } 
    else if ( !strncasecmp ( p, "&lt;/math&gt;", 13 ) ) 
    {
        while ( *p != '\0' && !isspace ( *p ) && *p != ';' ) p++;
        if ( *p == ';' ) {
            p++;
        }
        while ( *p != '\0' && !isspace ( *p ) && *p != ';' ) p++;
        if ( *p == ';' ) {
            p++;
        }
        found = 1;
        math_found = 0;
    } 
    else if ( !strncasecmp ( p, "&lt;code&gt;", 12 ) ) 
    {
        while ( *p != '\0' && !isspace ( *p ) && *p != ';' ) p++;
        if ( *p == ';' ) {
            p++;
        }
        while ( *p != '\0' && !isspace ( *p ) && *p != ';' ) p++;
        if ( *p == ';' ) {
            p++;
        }
        found = 1;
        code_found = 1;
    } 
    else if ( !strncasecmp ( p, "&lt;/code&gt;", 13 ) ) 
    {
        while ( *p != '\0' && !isspace ( *p ) && *p != ';' ) p++;
        if ( *p == ';' ) {
            p++;
        }
        while ( *p != '\0' && !isspace ( *p ) && *p != ';' ) p++;
        if ( *p == ';' ) {
            p++;
        }
        found = 1;
        code_found = 0;
    } 
    else if ( ( !strncasecmp ( p, "&lt;", 4 ) ) && ( ( math_found ) || ( html_found ) ) ) 
    { /* < */
        while ( *p != '\0' && !isspace ( *p ) && *p != ';' ) p++;
        if ( *p == ';' ) {
            p++;
        }
        found = 1;
    } 
    else if ( ( !strncasecmp ( p, "&gt;", 4 ) ) && ( ( math_found ) || ( html_found ) ) ) 
    { /* > */
        while ( *p != '\0' && !isspace ( *p ) && *p != ';' ) p++;
        if ( *p == ';' ) {
            p++;
        }
        found = 1;
    } 
    else if ( ( !strncasecmp ( p, "&quot;", 6 ) ) && ( ( math_found ) || ( html_found ) ) ) 
    { /* " */
        while ( *p != '\0' && !isspace ( *p ) && *p != ';' ) p++;
        if ( *p == ';' ) {
            p++;
        }
        found = 1;
    } 
    else if ( *p == '%' ) 
    {
        while ( *p != '\0' && !isspace ( *p ) ) p++;
        found = 1;
    }

    if ( found ) 
    {
        url = malloc ( sizeof ( char ) * ( ( p - start ) + 2 ) );
        memset ( url, 0, sizeof ( char ) * ( ( p - start ) + 2 ) );
        strncpy ( url, start, p - start );
        *start = '\0';

        *skip_chars = p - start;

        if ( !strncasecmp ( url, "&lt;html&gt;", 12 ) ) { }
        else if ( !strncasecmp ( url, "&lt;/html&gt;", 13 ) ) { }
        else if ( !strncasecmp ( url, "&lt;math&gt;", 12 ) ) { asprintf ( &result,"<math xmlns=\"http://www.w3.org/1998/Math/MathML\">" ); }
        else if ( !strncasecmp ( url, "&lt;/math&gt;", 13 ) ) { asprintf ( &result, "</math><p>" ); }
        else if ( !strncasecmp ( url, "&lt;code&gt;", 12 ) ) { }
        else if ( !strncasecmp ( url, "&lt;/code&gt;", 13 ) ) { }
        else if ( ( !strncasecmp ( url, "&lt;", 4 ) ) && ( ( math_found ) || ( html_found ) ) ) { asprintf ( &result, "<" ); } 
        else if ( ( !strncasecmp ( url, "&gt;", 4 ) ) && ( ( math_found ) || ( html_found ) ) ) { asprintf ( &result, ">" ); } 
        else if ( ( !strncasecmp ( url, "&quot;", 6 ) ) && ( ( math_found ) || ( html_found ) ) ) { asprintf ( &result, "\"" ); } 
        else if ( *url == '%' ) 
        {
            if ( *skip_chars == 1 ) asprintf ( &result, "%s", "%" );
            else asprintf ( &result, "&%s;", url+1 );
        }
        return result;
    }
    return NULL;
}

