#include <didi.h>

#ifndef _HAVE_WIKI_HEADER
#define _HAVE_WIKI_HEADER

typedef struct WikiPageList WikiPageList;

struct WikiPageList {
    char   *name;
    time_t  mtime;
};

void
wiki_handle_http_request ( HttpRequest *req );

void
wiki_show_header ( HttpResponse *res, char *page_title, char *page_title2, int want_edit, int want_login );

void
wiki_show_footer ( HttpResponse *res );

void
wiki_print_data_as_html ( HttpResponse *res, char *raw_page_data );

int
wiki_init ( char *didiwiki_home );


#endif
