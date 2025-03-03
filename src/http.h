#include <didi.h>

#ifndef _HAVE_HTTP_HEADER
#define _HAVE_HTTP_HEADER

typedef struct HttpResponse     HttpResponse;
typedef struct HttpRequest      HttpRequest;
typedef struct HttpRequestParam HttpRequestParam;

HttpRequest*
http_server ( int iPort );

HttpRequest*
http_request_new ( void );

void
http_request_param_showall ( HttpRequest *req, HttpResponse *res );

char*
http_request_param_get ( HttpRequest *req, char *key );

char*
http_request_get_uri ( HttpRequest *req );

char*
http_request_get_path_info ( HttpRequest *req );

char*
http_request_get_query_string ( HttpRequest *req );


HttpResponse*
http_response_new ( HttpRequest *req );

void
http_response_printf ( HttpResponse *res, const char *format, ... );

void
http_response_printf_alloc_buffer ( HttpResponse *res, int bytes );

void
http_response_set_content_type ( HttpResponse *res, char *type );

void
http_response_set_status ( HttpResponse *res,
                           int           status_code,
                           char         *status_desc ) ;

void
http_response_set_data ( HttpResponse *res, void *data, int data_len );

void
http_response_append_header ( HttpResponse *res, char *header );

void
http_response_send_headers ( HttpResponse *res );

void
http_response_send ( HttpResponse *res );


#endif
