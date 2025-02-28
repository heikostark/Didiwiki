#if defined __MINGW32__
#define mode_t unsigned short
static int my_mkdir ( const char *path, mode_t mode )
{
    return mkdir ( path );
}
#else  /* Annahme: Unix */
static int my_mkdir ( const char *path, mode_t mode )
{
    return mkdir ( path,mode );
}
#endif

static int  /* Compare function for compare Changes */
changes_compar ( const struct dirent **d1, const struct dirent **d2 )
{
    struct stat st1, st2;
    stat ( ( *d1 )->d_name, &st1 );
    stat ( ( *d2 )->d_name, &st2 );
    if ( st1.st_mtime > st2.st_mtime ) return 1;
    else return -1;
}

static char *
file_read ( char *filename )
{
    struct stat st;
    FILE*       fp;
    char        *str = "";
    int         len;

    /* Get the file size. */
    if ( stat ( filename, &st ) ) return NULL;
    if ( ! ( fp = fopen ( filename, "rb" ) ) ) return NULL;
    str = malloc ( sizeof ( char ) * ( st.st_size + 1 ) );
    len = fread ( str, 1, st.st_size, fp );
    if ( len >= 0 ) str[len] = '\0';
    fclose ( fp );
    return str;
}

static char *
file_read_bin ( char *filename, int *len )
{
    struct stat st;
    FILE*       fp;
    char        *str = "";

    /* Get the file size. */
    if ( stat ( filename, &st ) ) return NULL;
    if ( ! ( fp = fopen ( filename, "rb" ) ) ) return NULL;
    str = malloc ( sizeof ( char ) * ( st.st_size + 1 ) );
    *len = fread ( str, 1, st.st_size, fp );
    fclose ( fp );
    return str;
}

static int
file_write ( char *filename, char *data )
{
    FILE*       fp;
    int         bytes_written = 0;
    int         len           = strlen ( data )+1;

    if ( ! ( fp = fopen ( filename, "wb" ) ) ) return -1;
    while ( len > 0 ) {
        bytes_written = fwrite ( data, sizeof ( char ), len, fp );
        len = len - bytes_written;
        data = data + bytes_written;
    }
    fclose ( fp );
    return 1;
}

static int
file_copy ( char *dir, char *filename, char *dir2, char *filename2 )
{
    struct stat st;
    FILE*       fp;
    char        *data = "";
    int         len;
    int         bytes_written = 0;

    /* read file */
    chdir ( dir );
    if ( stat ( filename, &st ) ) return -1;
    data = malloc ( sizeof ( char ) * ( st.st_size + 1 ) );
    if ( data==NULL ) return -1;

    if ( ! ( fp = fopen ( filename, "rb" ) ) ) {
        free ( data );
        return -1;
    }
    len = fread ( data, sizeof ( char ), st.st_size, fp );
    fclose ( fp );

    /* write file */
    chdir ( dir2 );
    if ( ! ( fp = fopen ( filename2, "wb" ) ) ) {
        free ( data );
        return -1;
    }
    while ( len > 0 ) {
        bytes_written = fwrite ( data, sizeof ( char ), len, fp );
        len = len - bytes_written;
        data = data + bytes_written;
    }
    fclose ( fp );

    return 1;
}
