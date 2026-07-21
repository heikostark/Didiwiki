# Didiwiki

A small, simple, and lightweight personal wiki system written in C.

## Overview

**Didiwiki** is a minimalist wiki engine with a built-in web server, specifically designed for personal use. It serves as a tool for managing notes, todo lists, and other personal information. The project prioritizes simplicity, speed, and ease of use – without complex dependencies or external files.

## Features

- **Built-in Web Server**: No separate HTTP server required
- **WikiWikiWeb Syntax**: Simple and familiar wiki formatting language
- **Lightweight**: Written in C for minimal memory footprint and compact binaries
- **Personal-Focused**: Ideal for individual users and small teams
- **Simple Installation**: Compile and run – that's it
- **REST-like API**: Programmatic access to wiki pages
- **Page Management**: Create, edit, delete, and organize pages
- **Search & Categorization**: Simple search functionality and content categorization
- **Custom Styling**: CSS support via `styles.css`

## Technical Stack

- **Language:** C
- **License:** GPLv3
- **Runtime:** Standalone HTTP server with fork-based architecture
- **Platforms:** Unix/Linux (with portable C code)

## Repository Structure

```
src/
  didi.c          Main entry point, CLI argument processing
  wiki.c          Wiki engine, HTTP request handler, page rendering
  http.c          HTTP server and request parsing
  libfile.c       File operations (read, write, copy)
  libstring.c     String processing and utility functions
  libshow.c       HTML rendering and page display
  util.c          General utility functions
  wikitext.h      Wikitext parsing and formatting definitions
  
configure.ac      Autoconf configuration
Makefile.am       Automake configuration
```

## How It Works

Didiwiki runs as a standalone daemon:

1. **Initialization** (`didi.c`): CLI parameters are processed (port, home directory, debug mode)
2. **Wiki Setup** (`wiki_init()`): Creates the necessary directory structure under `~/.didiwiki/`
3. **HTTP Server** (`http_server()`): Starts a fork-based server on the specified port
4. **Request Handling** (`wiki_handle_http_request()`): Each HTTP request is handled by a child process
5. **Rendering** (`wiki_print_data_as_html()`): Wiki text is converted to HTML with support for:
   - Bold, italic, superscript, subscript text
   - Headings, lists, tables
   - Links and images
   - Preformatted text (monospace)
   - Definition lists

## Installation

### Basic Steps

```bash
./configure
make
make install
```

Or run directly from the `src/` directory:

```bash
./configure
make
./src/didiwiki
```

### Requirements

- GCC or another C compiler
- Standard C library with POSIX support
- Make/Autotools

### Configuration Options

```bash
./configure --prefix=/usr/local
```

See `INSTALL` for additional configuration options.

## Usage

### Starting the Server

```bash
# Standard port 8000, data storage in ~/.didiwiki
didiwiki

# Custom port
didiwiki --port 8080

# Custom home directory
didiwiki --home my_wiki

# Dropbox integration
didiwiki --dropbox

# Debug mode (no HTTP server, reads from stdin)
didiwiki debug
```

Then open your browser at `http://localhost:8000`

### Data Storage

Wiki pages are stored by default in `~/.didiwiki/`:

```
~/.didiwiki/
  WikiHome              Home page
  WikiHelp              Help page
  styles.css            (Optional) Custom CSS
  images/               Uploaded images
  includes/             Reusable page snippets
  secrets/              Protected pages
  categories/           Category indexes
  trash/                Deleted pages
```

You can override the storage location with the environment variable:

```bash
export DIDIWIKIHOME=/path/to/wiki
didiwiki
```

## Wiki Syntax

### Text Formatting

- `*bold text*` – **Bold**
- `/italic text/` – *Italic*
- `^superscript^` – Superscript
- `_subscript_` – Subscript

### Headings

```
= Heading 1 =
== Heading 2 ==
=== Heading 3 ===
```

### Lists

- `* Item` – Unordered list
- `# Item` – Ordered list
- Repeat for nesting: `** Nested item`

Supported list types:
- `•` - Bullet
- `o` - Circle style
- `*c`, `*d`, `*s`, `*n` - Circle, disc, square, none styles
- `#I`, `#i`, `#A`, `#a`, `#1`, `#01`, `#` - Roman (upper/lower), Latin (upper/lower), decimal styles

### Tables

```
| Column 1 | Column 2 | Column 3 |
| Data     | Data     | Data     |
```

### Links

- `WikiPageName` – Internal link (CamelCase)
- `[text](http://url.com)` – External link

### Images

- `[image]images/filename.jpg[/image]`

### Preformatted Text

Text with leading whitespace is formatted as `<pre>`:

```
  Code example
  with multiple lines
```

### Include and Special Directives

- `$pagename` – Include another page
- `$include:pagename` – Include from includes directory
- `$category:name` – Show category results
- `$search:term` – Show search results
- `$redirect:pagename` – Redirect to another page

### Definition Lists

```
; Term name
: Definition text
```

## REST API

Didiwiki provides a simple HTTP API for programmatic access:

```
GET /api/page/get?page=PageName
  Returns the raw wiki text of a page. HTTP 500 if not found.

POST /api/page/set?page=PageName&text=WikiText
  Creates or updates a page.

GET /api/page/exists?page=PageName
  Returns 'success' if the page exists, HTTP 500 otherwise.

GET /api/page/delete?page=PageName
  Deletes a page.

GET /api/pages
  Returns one line per page (format: PageName\tModified Date).
  Pages are sorted by most recently modified first.

GET /api/search?expr=SearchTerm
  Returns pages containing the search term.
```

## Implementation Notes

- **Memory Management**: Child processes (which handle requests) don't free memory since they are short-lived
- **Security**: The code is not production-ready. **Please do not run as root!**
- **Portability**: Uses `asprintf()`, which may not be available on some systems. Patches welcome.
- **Performance**: The wikitext parser could be optimized, but is currently fast enough
- **Styling**: Similar syntax to kwiki, which served as inspiration
- **Forking Model**: Simple lightweight forking server based on CVStrac's internal server
  - One child process per HTTP request
  - Processes are expected to be short-lived (< 1 second)

## Planned Features

See `todo` in the repository for planned improvements:

- HTTP Basic Authentication
- IP-based access control
- RSS feeds of changes/searches
- Parser rewrite for better performance
- Improved search functionality
- Interwiki-style links
- Page metadata and tagging
- GTD/Task management features
- Template customization

## License

GNU General Public License v3.0 (GPLv3) – see `COPYING` and `LICENSE`

## Authors

- **Matthew Allum** – Original author (<mallum@o-hand.com>)
- **Carsten Graeser** – Table support
- **Heiko Stark** – Redesign and maintenance

## Related Projects

- Original Didiwiki: https://github.com/pepa65/didiwiki
- Didiwiki-NG: https://github.com/iiiypuk/didiwiki-ng
- ciwiki: https://github.com/escapecode/ciwiki
- C2 Wiki entry: https://wiki.c2.com/?DidiWiki

## Troubleshooting

### "Unable to get home directory, is HOME set?"

Make sure the `HOME` environment variable is set:

```bash
export HOME=/home/username
didiwiki
```

### Port already in use

Use a different port with the `--port` flag:

```bash
didiwiki --port 9000
```

### Help page not updating after upgrade

If upgrading from an older version, delete the old help page:

```bash
rm ~/.didiwiki/WikiHelp
```

The current version will be created on the next start.

### Segmentation faults

You can debug segmentation faults by running in debug mode:

```bash
didiwiki debug < request.txt
```

This runs the server in a single process, reading HTTP requests from stdin.

## Contributing

Interested in improvements? Check the `todo` file or open an issue in the repository. Patches are welcome!

## Quick Start Example

```bash
# Clone the repository
git clone https://github.com/heikostark/Didiwiki.git
cd Didiwiki

# Build the project
./configure
make

# Start the wiki server
./src/didiwiki

# Open in browser
# Visit http://localhost:8000
```

The wiki will create the necessary directories and default pages automatically. Start editing and creating pages right away!
