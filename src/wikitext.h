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

#ifndef _HAVE_WIKI_TEXT_HEADER
#define _HAVE_WIKI_TEXT_HEADER


#define PAGEHEADER \
\
"<div class='container'>"\
"\n"\
"<div class='header'>\n"\
"<form method='get' action='/Search'>\n"\
"   <table border='0' width='100%%'>\n"\
"   <tr>\n"\
"       <td align='left' ><strong>%s</strong>%s %s%s%s %s%s%s %s%s%s %s%s%s</td>\n"\
"       <td align='right' >\n"\
"           <a href='/WikiHome' title='Visit Wiki home page. [alt-h]' accesskey='h'>Home</a> |\n"\
"           <a href='/Create' title='Create a new wiki page by title. [alt-n]' accesskey='n'>New</a> | \n"\
"           <a href='/Search' title='Menu for wiki database.'>Menu</a> | \n"\
"           <a href='/Tools' title='Tools for entrys in wiki database.'>Tools</a> | \n"\
"           <a href='/Login' title='Login in wiki subdir. [alt-l]' accesskey='l'>Login</a> | \n"\
"           <a href='/WikiHelp' title='Get help on wiki usage and formatting. [alt-?]' accesskey='?'>Help</a> |\n"\
"           <input type='text' name='expr' size='15' title='Enter text to search for and press return.' /> </td>\n"\
"   </tr>\n"\
"   </table>\n"\
"</form>\n"\
"</div>\n"\
"\n"\
"<div class='data'>\n"


#define PAGEFOOTER \
\
"</div>\n"\
"\n"\
"<div class='footer'>Wiki, <a href='https://starkrats.de'>https://starkrats.de</a></div>\n"\
"\n"\
"</div>\n"

#define CREATEFORM \
\
"<form method=POST action='/Create'>\n"\
"   <input type='text' name='title' />\n"\
"   <p><input type='submit' value='Create' /></p>\n"\
"</form>\n"

#define TOOLSFORM \
\
"<form method=POST action='/Replace'>\n"\
"   From: <input type='text' name='from' />\n"\
"   To: <input type='text' name='to' />\n"\
"   <p><input type='submit' value='Replace' /></p>\n"\
"</form>\n"

#define LOGINFORM \
\
"<form method=POST action='/Login'>\n"\
"   <input type='text' name='title' />\n"\
"   <p><input type='submit' value='Login' /></p>\n"\
"</form>\n"

#define EDITFORM \
\
"<form method=POST action='%s' name='editform'>\n"\
"   <p><input type='submit' value='Save secret' name='Savesecret'> --- \n"\
"   <input type='submit' value='Save include' name='Saveinclude'> --- \n"\
"   <input type='submit' value='Save' name='Save'></p>\n"\
"   <textarea name='wikitext' rows='40' cols='80' wrap='virtual' maxlength='65535'>%s</textarea>\n"\
"   <p><input type='submit' value='Save secret' name='Savesecret'> --- \n"\
"   <input type='submit' value='Save include' name='Saveinclude'> --- \n"\
"   <input type='submit' value='Save' name='Save'></p>\n"\
"</form>\n"\
"<script language='JavaScript'>\n"\
"   <!--\n"\
"   /* Give the textarea focus ...less mouse usage but may annoy people ?*/ \n"\
"   document.editform.wikitext.focus()\n"\
"   document.editform.wikitext.scrollTop = document.editform.wikitext.scrollHeight;\n"\
"   //-->\n"\
"</script>\n"


#define HOMETEXT \
\
"==Welcome to Wiki\n"\
"\n"\
"Wiki is a small and simple [http://en.wikipedia.org/wiki/Wiki|WikiWikiWeb]\n"\
"Implementaion. Its intended for personal note taking, Todo lists and any other uses you can think of.\n"\
"\n"\
"To learn more about what a [http://www.c2.com/cgi/wiki?WikiWikiWeb|WikiWikiWeb] is, read about [http://www.c2.com/cgi/wiki?WhyWikiWorks|WhyWikiWorks] and the [http://www.c2.com/cgi/wiki?WikiNature|WikiNature]. Also, consult the [http://www.c2.com/cgi/wiki?WikiWikiWebFaq|WikiWikiWebFaq].\n"\
"\n"\
"For an example how a WikiWiki entry looks in text form you can edit this page. Also see WikiHelp for infomation on usage and formatting rules.\n"\
"\n"\
"---\n"\
"\n"\
"[category:Wiki]\n"

#define HELPTEXT \
\
"=Quick Guide \n"\
"Top toolbar usage\n"\
"* Home : Takes you to the wiki front page\n"\
"* New : Creates a new wiki page by title\n"\
"* Menu : Allows you to see all pages\n"\
"* Replace : Replace entrys\n"\
"* Login : Login into a secret page\n"\
"* Help : Takes you to this help page\n"\
"\n"\
"Use the text entry box to perform a very simple keyword search on the\n"\
"Wiki contents. Hit return to activate the search.\n"\
"\n"\
"----\n"\
"=Formatting rules\n"\
"\n"\
"=Top Level Heading\n"\
"  =Top Level Heading (H1)\n"\
"==Second Level Heading\n"\
"  ==Second Level Heading (H2)\n"\
"More levels \n"\
"  === (H3), ==== (H4) etc\n"\
"----\n"\
"The horizontal lines in this page are made with 3 or more dashes:\n"\
" --- Horizonal line\n"\
"---\n"\
"\n"\
"Paragraphs are seperated by an empty line\n"\
"\n"\
"Like this. Another paragraph.\n"\
"\n"\
" Paragraphs are separated by a blank line.\n"\
"\n"\
" Like this. Another paragraph.\n"\
"----\n"\
"*Bold text*, /italic text/, _underscore text_ and ^superscript^ .\n"\
"  *Bold text*, /italic text/, _underscore text_ and ^superscript^ .\n"\
"/*Combination of bold and italics*/\n"\
"  /*Combination of bold and italics*/\n"\
"---\n"\
"\n"\
"External links begin with http://, like http://www.freepan.org\n"\
" External links begin with http://, like http://www.freepan.org\n"\
"Forced wiki [links] are a alphanumeric string surrounded by square brackets.\n"\
" Forced wiki [links] are a alphanumeric string surrounded by square brackets.\n"\
"Named http links have text with an !http:// link inside, like [http://www.freepan.org|FreePAN]\n"\
" Named http links have text with an !http:// link inside, like [http://www.freepan.org|FreePAN]\n"\
"---\n"\
"\n"\
"Links to images display the image:\n"\
"\n"\
"[image:http://www.google.com/images/logo.gif]\n"\
" [image:http://www.google.com/images/logo.gif]\n"\
"---\n"\
" Create [WikiInclude] to include this with $WikiInclude\n"\
"$WikiInclude\n"\
"---\n"\
"Unordered lists begin with a '* '. The number of asterisks determines the level:\n"\
"* foo\n"\
"* bar\n"\
"** boom\n"\
"** bam\n"\
"* baz\n"                                                                \
"\n"                                                                     \
"  * foo\n"                                                              \
"  * bar\n"                                                              \
"  ** boom\n"                                                            \
"  ** bam\n"                                                             \
"  * baz\n"                                                              \
"\n"                                                                     \
"Ordered lists work the same, but use a '#'\n"                           \
"\n"                                                                     \
"---\n"                                                                 \
"Tables begin the line with a '|' and before every new column after.\n"  \
"\n"                                                                     \
"| row:1,col:1 | row:1,col:2\n"                                          \
"| row:2,col:1 | row:2,col:2\n"                                          \
" | row:1,col:1 | row:1,col:2\n"                                         \
" | row:2,col:1 | row:2,col:2\n"                                         \
"---\n"                                                                  \
"<Inline> <html> <is> <escaped>.\n"                                      \
"\n"                                                                     \
"---\n"                                                                 \
"Lines prefixed with a space are unformatted ( Like examples on this page )\n" \
"\n"                                                                     \
"---\n" \
"Create category with [category:name] in page\n" \
"With click on the link, you can see all categorized pages\n"                                                                     \
"---\n"                                                              \
"[category:Wiki]\n" \

#define STYLESHEET \
\
"html,body {\n" \
"  font-family: Verdana, Arial, Helvetica, sans-serif;\n" \
"  font-size: 90%;  \n" \
"  color: #333333;\n" \
"  margin: 2px;\n" \
"  padding: 2px;\n" \
"  height: 99%;\n" \
"}\n" \
"\n" \
".container {" \
"    height: 100%;" \
"    display: flex;" \
"    flex-direction: column;" \
"}\n" \
"\n" \
".header {\n" \
"    background-color: #eef;\n" \
"    border: 1px solid #aaa;\n" \
"    padding: 5px;\n" \
"    margin-bottom:8px;\n" \
"}\n" \
".header input {\n" \
"    margin:0px;\n" \
"    padding: 0;\n" \
"    background-color: White;\n" \
"    border: 1px solid #aaaaaa;\n" \
"    font-size: 90%;\n" \
"}\n" \
"\n" \
".data{" \
"    flex: 1;" \
"    overflow: auto;" \
"}\n" \
"\n" \
".footer {\n" \
"    font-size: 12px;\n" \
"    color: #aaa;\n" \
"    margin-top:8px;\n" \
"    width: 100%;\n" \
"    text-align: center;\n" \
"}\n" \
"\n" \
"table.wikitable { background-color: #fff; border: 1px solid #aaa; }\n" \
"table.wikitable td { background-color: #fff; border: 1px solid #aaa; }\n" \
"\n" \
"pre {\n" \
"    font-family: monospace;\n" \
"    background-color: #eee;\n" \
"    padding: 2px;\n" \
"    padding-left: 10px;\n" \
"    margin-left: 20px;\n" \
"    margin-right: 20px;\n" \
"}\n" \
"\n" \
"dt {\n" \
"    font-weight: bold;\n" \
"}\n" \
"\n" \
"dd {\n" \
"    padding: 2px;\n" \
"    padding-left: 10px;\n" \
"    margin-left: 20px;\n" \
"    margin-right: 20px;\n" \
"}\n" \
"\n" \
"hr {\n" \
" height: 1px;\n" \
" color: #aaaaaa;\n" \
" background-color: #aaaaaa;\n" \
" border: 0;\n" \
" margin: 0.2em 5px 0.2em 5px;\n" \
"}\n" \
"\n" \
"form {\n" \
"    border: none;\n" \
"    margin: 0;\n" \
"}\n" \
"\n" \
"textarea {\n" \
"    border: 1px solid #aaaaaa;\n" \
"    color: Black;\n" \
"    background-color: white;\n" \
"    width: 100%;\n" \
"    padding: 0.1em;\n" \
"    overflow: auto;\n" \
"}\n" \
"\n" \
"input {\n" \
"    margin-top:1px;\n" \
"    padding: 0 0.4em !important;\n" \
"    background-color: White;\n" \
"    border: 1px solid #aaaaaa;\n" \
"}\n" \
"\n" \
"a, ulink{\n" \
"   color: #000;\n" \
"   text-decoration:none;\n" \
"   border-bottom: 1px #333 dotted;\n" \
"   display: inline;\n" \
" }\n" \
"\n" \
"a:hover {\n" \
"   color: #000;\n" \
"   text-decoration: None;\n" \
"   border-bottom: 1px #333 solid;\n" \
"   }\n" \
"\n" \
"a:visited {\n" \
"   color: #333;\n" \
"}\n" \
"\n" \
"/* Tooltip container */\n" \
".tooltip {\n" \
"  position: relative;\n" \
"  display: inline-block;\n" \
"  border-bottom: 1px dotted black; /* If you want dots under the hoverable text */\n" \
"}\n" \
"\n" \
"/* Tooltip text */\n" \
".tooltip .tooltiptext {\n" \
"  visibility: hidden;\n" \
"  width: 120px;\n" \
"  background-color: black;\n" \
"  color: #fff;\n" \
"  text-align: center;\n" \
"  padding: 5px 0;\n" \
"  border-radius: 6px;\n" \
"\n" \
"  /* Position the tooltip text - see examples below! */\n" \
"  position: absolute;\n" \
"  z-index: 1;\n" \
"}\n" \
"\n" \
"/* Show the tooltip text when you mouse over the tooltip container */\n" \
".tooltip:hover .tooltiptext {\n" \
"  visibility: visible;\n" \
"}\n" \
"\n"

/* use xdd -i favicon.ico to generate */

unsigned char FaviconData[] = {
    0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x10, 0x10, 0x02, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xb0, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x28, 0x00,
    0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xee, 0x00, 0xff, 0xff,
    0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xba, 0xab, 0x00, 0x00, 0x92, 0xab,
    0x00, 0x00, 0xaa, 0x9b, 0x00, 0x00, 0xbb, 0xaf, 0x00, 0x00, 0xba, 0xab,
    0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x8d, 0x1b,
    0x00, 0x00, 0xb5, 0x6b, 0x00, 0x00, 0xb5, 0x6b, 0x00, 0x00, 0xb7, 0x6f,
    0x00, 0x00, 0x8d, 0x1b, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned int FaviconDataLen = 198;


#endif
