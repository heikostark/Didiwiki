#include "didi.h"
#include "getopt.h"

static int debug;

void
usage ( char *progname )
{
    fprintf ( stderr, "usage..\n" );
}

int
main ( int argc, char **argv )
{
    HttpRequest  *req  = NULL;
    int           port = 8000;
    int           c;
    char          *didiwiki_home = NULL;

    debug = 0;

    while ( 1 ) {
        static struct option long_options[] = {
            {"debug", no_argument,       0, 'd'},
            {"port",  required_argument, 0, 'p'},
            {"home",  required_argument, 0, 'h'},
            {"dropbox",  no_argument, 0, 's'},
            {0, 0, 0, 0}
        };

        /* getopt_long stores the option index here */
        int option_index = 0;

        c = getopt_long ( argc, argv, "dp:h:", long_options, &option_index );

        /* detect the end of the options */
        if ( c == -1 ) break;

        switch ( c ) {
        case 0 :
            break;
        case 'd' :
            debug = 1;
            break;
        case 'p' :
            port = atoi ( optarg );
            break;
        case 'h' :
            didiwiki_home = optarg;
            break;
        case 's' :
            didiwiki_home = "Dropbox/.didiwiki";
            break;
        default :
            abort ();
        }
    }

    if ( didiwiki_home ) wiki_init ( didiwiki_home );
    else wiki_init ( ".didiwiki" );


    if ( debug ) {
        req = http_request_new(); 	/* reads request from stdin */
    } else {
        req = http_server ( port ); /* forks here */
    }

    wiki_handle_http_request ( req );
    return 0;
}
