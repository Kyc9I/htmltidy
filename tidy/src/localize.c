/* localize.c -- text strings and routines to handle errors and general messages

  (c) 1998-2004 (W3C) MIT, ERCIM, Keio University
  Portions Copyright University of Toronto
  See tidy.h and access.h for the copyright notice.

  You should only need to edit this file and tidy.c
  to localize HTML tidy. *** This needs checking ***
  
  CVS Info :

    $Author$ 
    $Date$ 
    $Revision$ 

*/

#include "tidy-int.h"
#include "lexer.h"
#include "streamio.h"
#include "message.h"
#include "tmbstr.h"
#include "utf8.h"

/* used to point to Web Accessibility Guidelines */
#define ACCESS_URL  "http://www.w3.org/WAI/GL"

/* points to the Adaptive Technology Resource Centre at the
** University of Toronto
*/
#define ATRC_ACCESS_URL  "http://www.aprompt.ca/Tidy/accessibilitychecks.html"

static const char *release_date = "1st August 2004";

ctmbstr ReleaseDate(void)
{
  return release_date;
}

static struct _msgfmt
{
    uint code;
    ctmbstr fmt;
} const msgFormat[] = 
{
/* ReportEncodingWarning */
  { ENCODING_MISMATCH,            "specified input encoding (%s) does not match actual input encoding (%s)" }, /* Warning */

/* ReportEncodingError */
  { VENDOR_SPECIFIC_CHARS,        "%s invalid character code %s"                                            }, /* Error */
  { INVALID_SGML_CHARS,           "%s invalid character code %s"                                            }, /* Error */
  { INVALID_UTF8,                 "%s invalid UTF-8 bytes (char. code %s)"                                  }, /* Error */
  { INVALID_UTF16,                "%s invalid UTF-16 surrogate pair (char. code %s)"                        }, /* Error */
  { INVALID_NCR,                  "%s invalid numeric character reference %s"                               }, /* Error */

/* ReportEntityError */
  { MISSING_SEMICOLON,            "entity \"%s\" doesn't end in ';'"                                        }, /* Warning in HTML, Error in XML/XHTML */
  { MISSING_SEMICOLON_NCR,        "numeric character reference \"%s\" doesn't end in ';'"                   }, /* Warning in HTML, Error in XML/XHTML */
  { UNESCAPED_AMPERSAND,          "unescaped & which should be written as &amp;"                            }, /* Warning in HTML, Error in XHTML */
  { UNKNOWN_ENTITY,               "unescaped & or unknown entity \"%s\""                                    }, /* Error */
  { APOS_UNDEFINED,               "named entity &apos; only defined in XML/XHTML"                           }, /* Error in HTML (should only occur for HTML input) */

/* ReportAttrError */

  /* attribute name */
  { INSERTING_ATTRIBUTE,          "%s inserting \"%s\" attribute"                                           }, /* Warning in CheckLINK, Error otherwise */
  { MISSING_ATTR_VALUE,           "%s attribute \"%s\" lacks value"                                         }, /* Warning in CheckUrl, Error otherwise */
  { UNKNOWN_ATTRIBUTE,            "%s unknown attribute \"%s\""                                             }, /* Error */
  { PROPRIETARY_ATTRIBUTE,        "%s proprietary attribute \"%s\""                                         }, /* Error */
  { JOINING_ATTRIBUTE,            "%s joining values of repeated attribute \"%s\""                          }, /* Error */
  { XML_ATTRIBUTE_VALUE,          "%s has XML attribute \"%s\""                                             }, /* Error (but deprecated) */

  /* attribute value */
  { XML_ID_SYNTAX,                "%s ID \"%s\" uses XML ID syntax"                                         }, /* Warning if XHTML, Error if HTML */
  { ATTR_VALUE_NOT_LCASE,         "%s attribute value \"%s\" must be lower case for XHTML"                  }, /* Error if XHTML input, Notice if HTML input and XHTML outout */
  { PROPRIETARY_ATTR_VALUE,       "%s proprietary attribute value \"%s\""                                   }, /* Error */
  { ANCHOR_NOT_UNIQUE,            "%s anchor \"%s\" already defined"                                        }, /* Error */

  /* attribute name, attribute value */
  { BAD_ATTRIBUTE_VALUE,          "%s attribute \"%s\" has invalid value \"%s\""                            }, /* Error */
  { BAD_ATTRIBUTE_VALUE_REPLACED, "%s attribute \"%s\" had invalid value \"%s\" and has been replaced"      }, /* Error */
  { INVALID_ATTRIBUTE,            "%s attribute name \"%s\" (value=\"%s\") is invalid"                      }, /* Error */

  /* attribute value, attribute name */
  { REPEATED_ATTRIBUTE,           "%s dropping value \"%s\" for repeated attribute \"%s\""                  }, /* Error */

  /* no arguments */
  { INVALID_XML_ID,               "%s cannot copy name attribute to id"                                     }, /* Warning */
  { UNEXPECTED_GT,                "%s missing '>' for end of tag"                                           }, /* Warning if HTML, Error if XML/XHTML */
  { UNEXPECTED_QUOTEMARK,         "%s unexpected or duplicate quote mark"                                   }, /* Error */
  { MISSING_QUOTEMARK,            "%s attribute with missing trailing quote mark"                           }, /* Error */
  { UNEXPECTED_END_OF_FILE_ATTR,  "%s end of file while parsing attributes"                                 }, /* Error */
  { ID_NAME_MISMATCH,             "%s id and name attribute value mismatch"                                 }, /* Error */
  { BACKSLASH_IN_URI,             "%s URI reference contains backslash. Typo?"                              }, /* Error */
  { FIXED_BACKSLASH,              "%s converting backslash in URI to slash"                                 }, /* Error */
  { ILLEGAL_URI_REFERENCE,        "%s improperly escaped URI reference"                                     }, /* Error */
  { ESCAPED_ILLEGAL_URI,          "%s escaping malformed URI reference"                                     }, /* Error */
  { NEWLINE_IN_URI,               "%s discarding newline in URI reference"                                  }, /* Error */
  { UNEXPECTED_EQUALSIGN,         "%s unexpected '=', expected attribute name"                              }, /* Error */
  { MISSING_IMAGEMAP,             "%s should use client-side image map"                                     }, /* Warning (but deprecated) */

/* ReportWarning */
  { NESTED_EMPHASIS,              "nested emphasis %s"                                                      }, /* Warning */
  { NESTED_QUOTATION,             "nested q elements, possible typo."                                       }, /* Warning */
  { OBSOLETE_ELEMENT,             "replacing obsolete element %s by %s"                                     }, /* Warning */
  { COERCE_TO_ENDTAG_WARN,        "<%s> is probably intended as </%s>"                                      }, /* Warning */

/* ReportNotice */
  { TRIM_EMPTY_ELEMENT,           "trimming empty %s"                                                       }, /* Notice */
  { REPLACING_ELEMENT,            "replacing %s by %s"                                                      }, /* Notice */

/* ReportError */
  { COERCE_TO_ENDTAG,             "<%s> is probably intended as </%s>"                                      }, /* Error */
  { REPLACING_UNEX_ELEMENT,       "replacing unexpected %s by %s"                                           }, /* Error */
  { MISSING_ENDTAG_FOR,           "missing </%s>"                                                           }, /* Error */
  { MISSING_ENDTAG_BEFORE,        "missing </%s> before %s"                                                 }, /* Error */
  { DISCARDING_UNEXPECTED,        "discarding unexpected %s"                                                }, /* Error */
  { NON_MATCHING_ENDTAG,          "replacing unexpected %s by </%s>"                                        }, /* Error */
  { TAG_NOT_ALLOWED_IN,           "%s isn't allowed in <%s> elements"                                       }, /* Error */
  { MISSING_STARTTAG,             "missing <%s>"                                                            }, /* Error */
  { UNEXPECTED_ENDTAG,            "unexpected </%s> in <%s>"                                                }, /* Error */
  { TOO_MANY_ELEMENTS,            "too many %s elements in <%s>"                                            }, /* Error */
  { USING_BR_INPLACE_OF,          "using <br> in place of %s"                                               }, /* Error */
  { INSERTING_TAG,                "inserting implicit <%s>"                                                 }, /* Error */
  { CANT_BE_NESTED,               "%s can't be nested"                                                      }, /* Error */
  { PROPRIETARY_ELEMENT,          "%s is not approved by W3C"                                               }, /* Error */
  { ILLEGAL_NESTING,              "%s shouldn't be nested"                                                  }, /* Error */
  { NOFRAMES_CONTENT,             "%s not inside 'noframes' element"                                        }, /* Error */
  { UNEXPECTED_END_OF_FILE,       "unexpected end of file %s"                                               }, /* Error */
  { ELEMENT_NOT_EMPTY,            "%s element not empty or not closed"                                      }, /* Error */
  { UNEXPECTED_ENDTAG_IN,         "unexpected </%s> in <%s>"                                                }, /* Error */
  { TOO_MANY_ELEMENTS_IN,         "too many %s elements in <%s>"                                            }, /* Error */
  { UNESCAPED_ELEMENT,            "unescaped %s in pre content"                                             }, /* Error (but deprecated) */

  /* no arguments */
  { DOCTYPE_AFTER_TAGS,           "<!DOCTYPE> isn't allowed after elements"                                 }, /* Error */
  { MISSING_TITLE_ELEMENT,        "inserting missing 'title' element"                                       }, /* Error */
  { INCONSISTENT_VERSION,         "HTML DOCTYPE doesn't match content"                                      }, /* Error */
  { MISSING_DOCTYPE,              "missing <!DOCTYPE> declaration"                                          }, /* Error */
  { CONTENT_AFTER_BODY,           "content occurs after end of body"                                        }, /* Error */
  { MALFORMED_COMMENT,            "adjacent hyphens within comment"                                         }, /* Error */
  { BAD_COMMENT_CHARS,            "expecting -- or >"                                                       }, /* Error */
  { BAD_CDATA_CONTENT,            "'<' + '/' + letter not allowed here"                                     }, /* Error */
  { INCONSISTENT_NAMESPACE,       "HTML namespace doesn't match content"                                    }, /* Error */
  { SPACE_PRECEDING_XMLDECL,      "removing whitespace preceding XML Declaration"                           }, /* Error */
  { MALFORMED_DOCTYPE,            "discarding malformed <!DOCTYPE>"                                         }, /* Error */
  { BAD_XML_COMMENT,              "XML comments can't contain --"                                           }, /* Error (but deprecated) */
  { DTYPE_NOT_UPPER_CASE,         "SYSTEM, PUBLIC, W3C, DTD, EN must be upper case"                         }, /* Error (but deprecated) */
  { ENCODING_IO_CONFLICT,         "Output encoding does not work with standard output"                      }, /* Error (but deprecated) */

/* ReportFatal */
  { SUSPECTED_MISSING_QUOTE,      "missing quote mark for attribute value"                                  }, /* Error? (not really sometimes) */
  { DUPLICATE_FRAMESET,           "repeated FRAMESET element"                                               }, /* Error */
  { UNKNOWN_ELEMENT,              "%s is not recognized!"                                                   }, /* Error */
  { UNEXPECTED_ENDTAG,            "unexpected </%s> in <%s>"                                                }, /* Error */

#if SUPPORT_ACCESSIBILITY_CHECKS

/* ReportAccess */
/* 
    List of error/warning messages.  The error code corresponds to
    the check that is listed in the AERT (HTML specifications).
*/
  { IMG_MISSING_ALT,                               "[1.1.1.1]: <img> missing 'alt' text."                                     }, /* Access */
  { IMG_ALT_SUSPICIOUS_FILENAME,                   "[1.1.1.2]: suspicious 'alt' text (filename)."                             }, /* Access */
  { IMG_ALT_SUSPICIOUS_FILE_SIZE,                  "[1.1.1.3]: suspicious 'alt' text (file size)."                            }, /* Access */
  { IMG_ALT_SUSPICIOUS_PLACEHOLDER,                "[1.1.1.4]: suspicious 'alt' text (placeholder)."                          }, /* Access */
  { IMG_ALT_SUSPICIOUS_TOO_LONG,                   "[1.1.1.10]: suspicious 'alt' text (too long)."                            }, /* Access */
  { IMG_MISSING_ALT_BULLET,                        "[1.1.1.11]: <img> missing 'alt' text (bullet)."                           }, /* Access */
  { IMG_MISSING_ALT_H_RULE,                        "[1.1.1.12]: <img> missing 'alt' text (horizontal rule)."                  }, /* Access */
  { IMG_MISSING_LONGDESC_DLINK,                    "[1.1.2.1]: <img> missing 'longdesc' and d-link."                          }, /* Access */
  { IMG_MISSING_DLINK,                             "[1.1.2.2]: <img> missing d-link."                                         }, /* Access */
  { IMG_MISSING_LONGDESC,                          "[1.1.2.3]: <img> missing 'longdesc'."                                     }, /* Access */
  { LONGDESC_NOT_REQUIRED,                         "[1.1.2.5]: 'longdesc' not required."                                      }, /* Access */
  { IMG_BUTTON_MISSING_ALT,                        "[1.1.3.1]: <img> (button) missing 'alt' text."                            }, /* Access */
  { APPLET_MISSING_ALT,                            "[1.1.4.1]: <applet> missing alternate content."                           }, /* Access */
  { OBJECT_MISSING_ALT,                            "[1.1.5.1]: <object> missing alternate content."                           }, /* Access */
  { AUDIO_MISSING_TEXT_WAV,                        "[1.1.6.1]: audio missing text transcript (wav)."                          }, /* Access */
  { AUDIO_MISSING_TEXT_AU,                         "[1.1.6.2]: audio missing text transcript (au)."                           }, /* Access */
  { AUDIO_MISSING_TEXT_AIFF,                       "[1.1.6.3]: audio missing text transcript (aiff)."                         }, /* Access */
  { AUDIO_MISSING_TEXT_SND,                        "[1.1.6.4]: audio missing text transcript (snd)."                          }, /* Access */
  { AUDIO_MISSING_TEXT_RA,                         "[1.1.6.5]: audio missing text transcript (ra)."                           }, /* Access */
  { AUDIO_MISSING_TEXT_RM,                         "[1.1.6.6]: audio missing text transcript (rm)."                           }, /* Access */
  { FRAME_MISSING_LONGDESC,                        "[1.1.8.1]: <frame> may require 'longdesc'."                               }, /* Access */
  { AREA_MISSING_ALT,                              "[1.1.9.1]: <area> missing 'alt' text."                                    }, /* Access */
  { SCRIPT_MISSING_NOSCRIPT,                       "[1.1.10.1]: <script> missing <noscript> section."                         }, /* Access */
  { ASCII_REQUIRES_DESCRIPTION,                    "[1.1.12.1]: ascii art requires description."                              }, /* Access */
  { IMG_MAP_SERVER_REQUIRES_TEXT_LINKS,            "[1.2.1.1]: image map (server-side) requires text links."                  }, /* Access */
  { MULTIMEDIA_REQUIRES_TEXT,                      "[1.4.1.1]: multimedia requires synchronized text equivalents."            }, /* Access */
  { IMG_MAP_CLIENT_MISSING_TEXT_LINKS,             "[1.5.1.1]: image map (client-side) missing text links."                   }, /* Access */
  { INFORMATION_NOT_CONVEYED_IMAGE,                "[2.1.1.1]: ensure information not conveyed through color alone (image)."  }, /* Access */
  { INFORMATION_NOT_CONVEYED_APPLET,               "[2.1.1.2]: ensure information not conveyed through color alone (applet)." }, /* Access */
  { INFORMATION_NOT_CONVEYED_OBJECT,               "[2.1.1.3]: ensure information not conveyed through color alone (object)." }, /* Access */
  { INFORMATION_NOT_CONVEYED_SCRIPT,               "[2.1.1.4]: ensure information not conveyed through color alone (script)." }, /* Access */
  { INFORMATION_NOT_CONVEYED_INPUT,                "[2.1.1.5]: ensure information not conveyed through color alone (input)."  }, /* Access */
  { COLOR_CONTRAST_TEXT,                           "[2.2.1.1]: poor color contrast (text)."                                   }, /* Access */
  { COLOR_CONTRAST_LINK,                           "[2.2.1.2]: poor color contrast (link)."                                   }, /* Access */
  { COLOR_CONTRAST_ACTIVE_LINK,                    "[2.2.1.3]: poor color contrast (active link)."                            }, /* Access */
  { COLOR_CONTRAST_VISITED_LINK,                   "[2.2.1.4]: poor color contrast (visited link)."                           }, /* Access */
  { DOCTYPE_MISSING,                               "[3.2.1.1]: <doctype> missing."                                            }, /* Access */
  { STYLE_SHEET_CONTROL_PRESENTATION,              "[3.3.1.1]: use style sheets to control presentation."                     }, /* Access */
  { HEADERS_IMPROPERLY_NESTED,                     "[3.5.1.1]: headers improperly nested."                                    }, /* Access */
  { POTENTIAL_HEADER_BOLD,                         "[3.5.2.1]: potential header (bold)."                                      }, /* Access */
  { POTENTIAL_HEADER_ITALICS,                      "[3.5.2.2]: potential header (italics)."                                   }, /* Access */
  { POTENTIAL_HEADER_UNDERLINE,                    "[3.5.2.3]: potential header (underline)."                                 }, /* Access */
  { HEADER_USED_FORMAT_TEXT,                       "[3.5.3.1]: header used to format text."                                   }, /* Access */
  { LIST_USAGE_INVALID_UL,                         "[3.6.1.1]: list usage invalid <ul>."                                      }, /* Access */
  { LIST_USAGE_INVALID_OL,                         "[3.6.1.2]: list usage invalid <ol>."                                      }, /* Access */
  { LIST_USAGE_INVALID_LI,                         "[3.6.1.4]: list usage invalid <li>."                                      }, /* Access */
  { INDICATE_CHANGES_IN_LANGUAGE,                  "[4.1.1.1]: indicate changes in language."                                 }, /* Access */
  { LANGUAGE_NOT_IDENTIFIED,                       "[4.3.1.1]: language not identified."                                      }, /* Access */
  { LANGUAGE_INVALID,                              "[4.3.1.2]: language attribute invalid."                                   }, /* Access */
  { DATA_TABLE_MISSING_HEADERS,                    "[5.1.2.1]: data <table> missing row/column headers (all)."                }, /* Access */
  { DATA_TABLE_MISSING_HEADERS_COLUMN,             "[5.1.2.2]: data <table> missing row/column headers (1 col)."              }, /* Access */
  { DATA_TABLE_MISSING_HEADERS_ROW,                "[5.1.2.3]: data <table> missing row/column headers (1 row)."              }, /* Access */
  { DATA_TABLE_REQUIRE_MARKUP_COLUMN_HEADERS,      "[5.2.1.1]: data <table> may require markup (column headers)."             }, /* Access */
  { DATA_TABLE_REQUIRE_MARKUP_ROW_HEADERS,         "[5.2.1.2]: data <table> may require markup (row headers)."                }, /* Access */
  { LAYOUT_TABLES_LINEARIZE_PROPERLY,              "[5.3.1.1]: verify layout tables linearize properly."                      }, /* Access */
  { LAYOUT_TABLE_INVALID_MARKUP,                   "[5.4.1.1]: invalid markup used in layout <table>."                        }, /* Access */
  { TABLE_MISSING_SUMMARY,                         "[5.5.1.1]: <table> missing summary."                                      }, /* Access */
  { TABLE_SUMMARY_INVALID_NULL,                    "[5.5.1.2]: <table> summary invalid (null)."                               }, /* Access */
  { TABLE_SUMMARY_INVALID_SPACES,                  "[5.5.1.3]: <table> summary invalid (spaces)."                             }, /* Access */
  { TABLE_SUMMARY_INVALID_PLACEHOLDER,             "[5.5.1.6]: <table> summary invalid (placeholder text)."                   }, /* Access */
  { TABLE_MISSING_CAPTION,                         "[5.5.2.1]: <table> missing <caption>."                                    }, /* Access */
  { TABLE_MAY_REQUIRE_HEADER_ABBR,                 "[5.6.1.1]: <table> may require header abbreviations."                     }, /* Access */
  { TABLE_MAY_REQUIRE_HEADER_ABBR_NULL,            "[5.6.1.2]: <table> header abbreviations invalid (null)."                  }, /* Access */
  { TABLE_MAY_REQUIRE_HEADER_ABBR_SPACES,          "[5.6.1.3]: <table> header abbreviations invalid (spaces)."                }, /* Access */
  { STYLESHEETS_REQUIRE_TESTING_LINK,              "[6.1.1.1]: style sheets require testing (link)."                          }, /* Access */
  { STYLESHEETS_REQUIRE_TESTING_STYLE_ELEMENT,     "[6.1.1.2]: style sheets require testing (style element)."                 }, /* Access */
  { STYLESHEETS_REQUIRE_TESTING_STYLE_ATTR,        "[6.1.1.3]: style sheets require testing (style attribute)."               }, /* Access */
  { FRAME_SRC_INVALID,                             "[6.2.1.1]: <frame> source invalid."                                       }, /* Access */
  { TEXT_EQUIVALENTS_REQUIRE_UPDATING_APPLET,      "[6.2.2.1]: text equivalents require updating (applet)."                   }, /* Access */
  { TEXT_EQUIVALENTS_REQUIRE_UPDATING_SCRIPT,      "[6.2.2.2]: text equivalents require updating (script)."                   }, /* Access */
  { TEXT_EQUIVALENTS_REQUIRE_UPDATING_OBJECT,      "[6.2.2.3]: text equivalents require updating (object)."                   }, /* Access */
  { PROGRAMMATIC_OBJECTS_REQUIRE_TESTING_SCRIPT,   "[6.3.1.1]: programmatic objects require testing (script)."                }, /* Access */
  { PROGRAMMATIC_OBJECTS_REQUIRE_TESTING_OBJECT,   "[6.3.1.2]: programmatic objects require testing (object)."                }, /* Access */
  { PROGRAMMATIC_OBJECTS_REQUIRE_TESTING_EMBED,    "[6.3.1.3]: programmatic objects require testing (embed)."                 }, /* Access */
  { PROGRAMMATIC_OBJECTS_REQUIRE_TESTING_APPLET,   "[6.3.1.4]: programmatic objects require testing (applet)."                }, /* Access */
  { FRAME_MISSING_NOFRAMES,                        "[6.5.1.1]: <frameset> missing <noframes> section."                        }, /* Access */
  { NOFRAMES_INVALID_NO_VALUE,                     "[6.5.1.2]: <noframes> section invalid (no value)."                        }, /* Access */
  { NOFRAMES_INVALID_CONTENT,                      "[6.5.1.3]: <noframes> section invalid (content)."                         }, /* Access */
  { NOFRAMES_INVALID_LINK,                         "[6.5.1.4]: <noframes> section invalid (link)."                            }, /* Access */
  { REMOVE_FLICKER_SCRIPT,                         "[7.1.1.1]: remove flicker (script)."                                      }, /* Access */
  { REMOVE_FLICKER_OBJECT,                         "[7.1.1.2]: remove flicker (object)."                                      }, /* Access */
  { REMOVE_FLICKER_EMBED,                          "[7.1.1.3]: remove flicker (embed)."                                       }, /* Access */
  { REMOVE_FLICKER_APPLET,                         "[7.1.1.4]: remove flicker (applet)."                                      }, /* Access */
  { REMOVE_FLICKER_ANIMATED_GIF,                   "[7.1.1.5]: remove flicker (animated gif)."                                }, /* Access */
  { REMOVE_BLINK_MARQUEE,                          "[7.2.1.1]: remove blink/marquee."                                         }, /* Access */
  { REMOVE_AUTO_REFRESH,                           "[7.4.1.1]: remove auto-refresh."                                          }, /* Access */
  { REMOVE_AUTO_REDIRECT,                          "[7.5.1.1]: remove auto-redirect."                                         }, /* Access */
  { ENSURE_PROGRAMMATIC_OBJECTS_ACCESSIBLE_SCRIPT, "[8.1.1.1]: ensure programmatic objects are accessible (script)."          }, /* Access */
  { ENSURE_PROGRAMMATIC_OBJECTS_ACCESSIBLE_OBJECT, "[8.1.1.2]: ensure programmatic objects are accessible (object)."          }, /* Access */
  { ENSURE_PROGRAMMATIC_OBJECTS_ACCESSIBLE_APPLET, "[8.1.1.3]: ensure programmatic objects are accessible (applet)."          }, /* Access */
  { ENSURE_PROGRAMMATIC_OBJECTS_ACCESSIBLE_EMBED,  "[8.1.1.4]: ensure programmatic objects are accessible (embed)."           }, /* Access */
  { IMAGE_MAP_SERVER_SIDE_REQUIRES_CONVERSION,     "[9.1.1.1]: image map (server-side) requires conversion."                  }, /* Access */
  { SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_MOUSE_DOWN,  "[9.3.1.1]: <script> not keyboard accessible (onMouseDown)."               }, /* Access */
  { SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_MOUSE_UP,    "[9.3.1.2]: <script> not keyboard accessible (onMouseUp)."                 }, /* Access */
  { SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_CLICK,       "[9.3.1.3]: <script> not keyboard accessible (onClick)."                   }, /* Access */
  { SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_MOUSE_OVER,  "[9.3.1.4]: <script> not keyboard accessible (onMouseOver)."               }, /* Access */
  { SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_MOUSE_OUT,   "[9.3.1.5]: <script> not keyboard accessible (onMouseOut)."                }, /* Access */
  { SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_MOUSE_MOVE,  "[9.3.1.6]: <script> not keyboard accessible (onMouseMove)."               }, /* Access */
  { NEW_WINDOWS_REQUIRE_WARNING_NEW,               "[10.1.1.1]: new windows require warning (_new)."                          }, /* Access */
  { NEW_WINDOWS_REQUIRE_WARNING_BLANK,             "[10.1.1.2]: new windows require warning (_blank)."                        }, /* Access */
  { LABEL_NEEDS_REPOSITIONING_BEFORE_INPUT,        "[10.2.1.1]: <label> needs repositioning (<label> before <input>)."        }, /* Access */
  { LABEL_NEEDS_REPOSITIONING_AFTER_INPUT,         "[10.2.1.2]: <label> needs repositioning (<label> after <input>)."         }, /* Access */
  { FORM_CONTROL_REQUIRES_DEFAULT_TEXT,            "[10.4.1.1]: form control requires default text."                          }, /* Access */
  { FORM_CONTROL_DEFAULT_TEXT_INVALID_NULL,        "[10.4.1.2]: form control default text invalid (null)."                    }, /* Access */
  { FORM_CONTROL_DEFAULT_TEXT_INVALID_SPACES,      "[10.4.1.3]: form control default text invalid (spaces)."                  }, /* Access */
  { REPLACE_DEPRECATED_HTML_APPLET,                "[11.2.1.1]: replace deprecated html <applet>."                            }, /* Access */
  { REPLACE_DEPRECATED_HTML_BASEFONT,              "[11.2.1.2]: replace deprecated html <basefont>."                          }, /* Access */
  { REPLACE_DEPRECATED_HTML_CENTER,                "[11.2.1.3]: replace deprecated html <center>."                            }, /* Access */
  { REPLACE_DEPRECATED_HTML_DIR,                   "[11.2.1.4]: replace deprecated html <dir>."                               }, /* Access */
  { REPLACE_DEPRECATED_HTML_FONT,                  "[11.2.1.5]: replace deprecated html <font>."                              }, /* Access */
  { REPLACE_DEPRECATED_HTML_ISINDEX,               "[11.2.1.6]: replace deprecated html <isindex>."                           }, /* Access */
  { REPLACE_DEPRECATED_HTML_MENU,                  "[11.2.1.7]: replace deprecated html <menu>."                              }, /* Access */
  { REPLACE_DEPRECATED_HTML_S,                     "[11.2.1.8]: replace deprecated html <s>."                                 }, /* Access */
  { REPLACE_DEPRECATED_HTML_STRIKE,                "[11.2.1.9]: replace deprecated html <strike>."                            }, /* Access */
  { REPLACE_DEPRECATED_HTML_U,                     "[11.2.1.10]: replace deprecated html <u>."                                }, /* Access */
  { FRAME_MISSING_TITLE,                           "[12.1.1.1]: <frame> missing title."                                       }, /* Access */
  { FRAME_TITLE_INVALID_NULL,                      "[12.1.1.2]: <frame> title invalid (null)."                                }, /* Access */
  { FRAME_TITLE_INVALID_SPACES,                    "[12.1.1.3]: <frame> title invalid (spaces)."                              }, /* Access */
  { ASSOCIATE_LABELS_EXPLICITLY,                   "[12.4.1.1]: associate labels explicitly with form controls."              }, /* Access */
  { ASSOCIATE_LABELS_EXPLICITLY_FOR,               "[12.4.1.2]: associate labels explicitly with form controls (for)."        }, /* Access */
  { ASSOCIATE_LABELS_EXPLICITLY_ID,                "[12.4.1.3]: associate labels explicitly with form controls (id)."         }, /* Access */
  { LINK_TEXT_NOT_MEANINGFUL,                      "[13.1.1.1]: link text not meaningful."                                    }, /* Access */
  { LINK_TEXT_MISSING,                             "[13.1.1.2]: link text missing."                                           }, /* Access */
  { LINK_TEXT_TOO_LONG,                            "[13.1.1.3]: link text too long."                                          }, /* Access */
  { LINK_TEXT_NOT_MEANINGFUL_CLICK_HERE,           "[13.1.1.4]: link text not meaningful (click here)."                       }, /* Access */
  { LINK_TEXT_NOT_MEANINGFUL_MORE,                 "[13.1.1.5]: link text not meaningful (more)."                             }, /* Access */
  { LINK_TEXT_NOT_MEANINGFUL_FOLLOW_THIS,          "[13.1.1.6]: link text not meaningful (follow this)."                      }, /* Access */
  { METADATA_MISSING,                              "[13.2.1.1]: Metadata missing."                                            }, /* Access */
  { METADATA_MISSING_LINK,                         "[13.2.1.2]: Metadata missing (link element)."                             }, /* Access */
  { METADATA_MISSING_REDIRECT_AUTOREFRESH,         "[13.2.1.3]: Metadata missing (redirect/auto-refresh)."                    }, /* Access */
  { SKIPOVER_ASCII_ART,                            "[13.10.1.1]: skip over ascii art."                                        }, /* Access */

#endif /* SUPPORT_ACCESSIBILITY_CHECKS */

  /* must be last */
  { 0,                                             NULL                                                                       }
};

static ctmbstr GetFormatFromCode(uint code)
{
    uint i;

    for (i = 0; msgFormat[i].fmt; ++i)
        if (msgFormat[i].code == code)
            return msgFormat[i].fmt;

    return NULL;
}

static char* LevelPrefix( TidyReportLevel level, char* buf, size_t count )
{
  *buf = 0;
  switch ( level )
  {
  case TidyInfo:
    tmbstrncpy( buf, "Info: ", count );
    break;
  case TidyWarning:
    tmbstrncpy( buf, "Warning: ", count );
    break;
  case TidyConfig:
    tmbstrncpy( buf, "Config: ", count );
    break;
  case TidyAccess:
    tmbstrncpy( buf, "Access: ", count );
    break;
  case TidyError:
    tmbstrncpy( buf, "Error: ", count );
    break;
  case TidyBadDocument:
    tmbstrncpy( buf, "Document: ", count );
    break;
  case TidyFatal:
    tmbstrncpy( buf, "panic: ", count );
    break;
  }
  return buf + tmbstrlen( buf );
}

/* Updates document message counts and
** compares counts to options to see if message
** display should go forward.
*/
static Bool UpdateCount( TidyDocImpl* doc, TidyReportLevel level )
{
  /* keep quiet after <ShowErrors> errors */
  Bool go = ( doc->errors < cfg(doc, TidyShowErrors) );

  switch ( level )
  {
  case TidyInfo:
    doc->infoMessages++;
    break;
  case TidyWarning:
    doc->warnings++;
    go = go && cfgBool( doc, TidyShowWarnings );
    break;
  case TidyConfig:
    doc->optionErrors++;
    break;
  case TidyAccess:
    doc->accessErrors++;
    break;
  case TidyError:
    doc->errors++;
    break;
  case TidyBadDocument:
    doc->docErrors++;
    break;
  case TidyFatal:
    /* Ack! */;
    break;
  }

  return go;
}

static char* ReportPosition(TidyDocImpl* doc, int line, int col, char* buf, size_t count)
{
    *buf = 0;

    /* Change formatting to be parsable by GNU Emacs */
    if ( cfgBool(doc, TidyEmacs) && cfgStr(doc, TidyEmacsFile) )
        tmbsnprintf(buf, count, "%s:%d:%d: ", 
                 cfgStr(doc, TidyEmacsFile), line, col);
    else /* traditional format */
        tmbsnprintf(buf, count, "line %d column %d - ", line, col);
    return buf + tmbstrlen( buf );
}

/* General message writing routine.
** Each message is a single warning, error, etc.
** 
** This routine will keep track of counts and,
** if the caller has set a filter, it will be 
** called.  The new preferred way of handling
** Tidy diagnostics output is either a) define
** a new output sink or b) install a message
** filter routine.
*/

static void messagePos( TidyDocImpl* doc, TidyReportLevel level,
                        int line, int col, ctmbstr msg, va_list args )
{
    char messageBuf[ 2048 ];
    Bool go = UpdateCount( doc, level );

    if ( go )
    {
        tmbvsnprintf(messageBuf, sizeof(messageBuf), msg, args);
        if ( doc->mssgFilt )
        {
            TidyDoc tdoc = tidyImplToDoc( doc );
            go = doc->mssgFilt( tdoc, level, line, col, messageBuf );
        }
    }

    if ( go )
    {
        char buf[ 64 ], *cp;
        if ( line > 0 && col > 0 )
        {
            ReportPosition(doc, line, col, buf, sizeof(buf));
            for ( cp = buf; *cp; ++cp )
                WriteChar( *cp, doc->errout );
        }

        LevelPrefix( level, buf, sizeof(buf) );
        for ( cp = buf; *cp; ++cp )
            WriteChar( *cp, doc->errout );

        for ( cp = messageBuf; *cp; ++cp )
            WriteChar( *cp, doc->errout );
        WriteChar( '\n', doc->errout );
    }
}

void message( TidyDocImpl* doc, TidyReportLevel level, ctmbstr msg, ... )
{
    va_list args;
    va_start( args, msg );
    messagePos( doc, level, 0, 0, msg, args );
    va_end( args );
}


void messageLexer( TidyDocImpl* doc, TidyReportLevel level, ctmbstr msg, ... )
{
    int line = ( doc->lexer ? doc->lexer->lines : 0 );
    int col  = ( doc->lexer ? doc->lexer->columns : 0 );

    va_list args;
    va_start( args, msg );
    messagePos( doc, level, line, col, msg, args );
    va_end( args );
}

void messageNode( TidyDocImpl* doc, TidyReportLevel level, Node* node,
                  ctmbstr msg, ... )
{
    int line = ( node ? node->line :
                 ( doc->lexer ? doc->lexer->lines : 0 ) );
    int col  = ( node ? node->column :
                 ( doc->lexer ? doc->lexer->columns : 0 ) );

    va_list args;
    va_start( args, msg );
    messagePos( doc, level, line, col, msg, args );
    va_end( args );
}

void tidy_out( TidyDocImpl* doc, ctmbstr msg, ... )
{
    if ( !cfgBool(doc, TidyQuiet) )
    {
        ctmbstr cp;
        char buf[ 2048 ];

        va_list args;
        va_start( args, msg );
        tmbvsnprintf(buf, sizeof(buf), msg, args);
        va_end( args );

        for ( cp=buf; *cp; ++cp )
          WriteChar( *cp, doc->errout );
    }
}

void ShowVersion( TidyDocImpl* doc )
{
    ctmbstr platform = "", helper = "";

#ifdef PLATFORM_NAME
    platform = PLATFORM_NAME;
    helper = " for ";
#endif

    tidy_out( doc, "\nHTML Tidy%s%s (release date: %s; built on %s, at %s)\n"
                   "See http://tidy.sourceforge.net/ for details.\n",
              helper, platform, release_date, __DATE__, __TIME__ );
}

void FileError( TidyDocImpl* doc, ctmbstr file, TidyReportLevel level )
{
    message( doc, level, "Can't open \"%s\"\n", file );
}

static char* TagToString(Node* tag, char* buf, size_t count)
{
    *buf = 0;
    if (tag)
    {
        if (tag->type == StartTag || tag->type == StartEndTag)
            tmbsnprintf(buf, count, "<%s>", tag->element);
        else if (tag->type == EndTag)
            tmbsnprintf(buf, count, "</%s>", tag->element);
        else if (tag->type == DocTypeTag)
            tmbsnprintf(buf, count, "<!DOCTYPE>");
        else if (tag->type == TextNode)
            tmbsnprintf(buf, count, "plain text");
        else if (tag->type == XmlDecl)
            tmbsnprintf(buf, count, "XML declaration");
        else if (tag->element)
            tmbsnprintf(buf, count, "%s", tag->element);
    }
    return buf + tmbstrlen(buf);
}

/* lexer is not defined when this is called */
void ReportUnknownOption( TidyDocImpl* doc, ctmbstr option )
{
    assert( option != NULL );
    message( doc, TidyConfig, "unknown option: %s", option );
}

/* lexer is not defined when this is called */
void ReportBadArgument( TidyDocImpl* doc, ctmbstr option )
{
    assert( option != NULL );
    message( doc, TidyConfig,
             "missing or malformed argument for option: %s", option );
}

static void NtoS(int n, tmbstr str)
{
    tmbchar buf[40];
    int i;

    for (i = 0;; ++i)
    {
        buf[i] = (tmbchar)( (n % 10) + '0' );

        n = n / 10;

        if (n == 0)
            break;
    }

    n = i;

    while (i >= 0)
    {
        str[n-i] = buf[i];
        --i;
    }

    str[n+1] = '\0';
}

void ReportEncodingWarning(TidyDocImpl* doc, uint code, uint encoding)
{
    switch(code)
    {
    case ENCODING_MISMATCH:
        messageLexer(doc, TidyWarning, GetFormatFromCode(code), 
                     CharEncodingName(doc->docIn->encoding),
                     CharEncodingName(encoding));
        doc->badChars |= BC_ENCODING_MISMATCH;
        break;
    }
}

void ReportEncodingError(TidyDocImpl* doc, uint code, uint c, Bool discarded)
{
    char buf[ 32 ] = {'\0'};

    ctmbstr action = discarded ? "discarding" : "replacing";
    ctmbstr fmt = GetFormatFromCode(code);

    /* An encoding mismatch is currently treated as a non-fatal error */
    switch (code)
    {
    case VENDOR_SPECIFIC_CHARS:
        NtoS(c, buf);
        doc->badChars |= BC_VENDOR_SPECIFIC_CHARS;
        break;

    case INVALID_SGML_CHARS:
        NtoS(c, buf);
        doc->badChars |= BC_INVALID_SGML_CHARS;
        break;

    case INVALID_UTF8:
        tmbsnprintf(buf, sizeof(buf), "U+%04X", c);
        doc->badChars |= BC_INVALID_UTF8;
        break;

#if SUPPORT_UTF16_ENCODINGS
    case INVALID_UTF16:
        tmbsnprintf(buf, sizeof(buf), "U+%04X", c);
        doc->badChars |= BC_INVALID_UTF16;
        break;
#endif

    case INVALID_NCR:
        NtoS(c, buf);
        doc->badChars |= BC_INVALID_NCR;
        break;
    }

    if (fmt)
        messageLexer( doc, TidyWarning, fmt, action, buf );
}

void ReportEntityError( TidyDocImpl* doc, uint code, ctmbstr entity, int c )
{
#pragma unused(c)

    ctmbstr entityname = ( entity ? entity : "NULL" );
    ctmbstr fmt = GetFormatFromCode(code);

    if (fmt)
        messageLexer( doc, TidyWarning, fmt, entityname );
}

void ReportAttrError(TidyDocImpl* doc, Node *node, AttVal *av, uint code)
{
    char const *name = "NULL", *value = "NULL";
    char tagdesc[64];
    ctmbstr fmt = GetFormatFromCode(code);

    assert( fmt != NULL );

    TagToString(node, tagdesc, sizeof(tagdesc));

    if (av)
    {
        if (av->attribute)
            name = av->attribute;
        if (av->value)
            value = av->value;
    }

    switch (code)
    {
    case UNKNOWN_ATTRIBUTE:
    case INSERTING_ATTRIBUTE:
    case MISSING_ATTR_VALUE:
    case XML_ATTRIBUTE_VALUE:
    case PROPRIETARY_ATTRIBUTE:
    case JOINING_ATTRIBUTE:
        messageNode(doc, TidyWarning, node, fmt, tagdesc, name);
        break;

    case BAD_ATTRIBUTE_VALUE:
    case BAD_ATTRIBUTE_VALUE_REPLACED:
    case INVALID_ATTRIBUTE:
        messageNode(doc, TidyWarning, node, fmt, tagdesc, name, value);
        break;

    case UNEXPECTED_QUOTEMARK:
    case MISSING_QUOTEMARK:
    case ID_NAME_MISMATCH:
    case BACKSLASH_IN_URI:
    case FIXED_BACKSLASH:
    case ILLEGAL_URI_REFERENCE:
    case ESCAPED_ILLEGAL_URI:
    case NEWLINE_IN_URI:
    case UNEXPECTED_GT:
    case INVALID_XML_ID:
    case UNEXPECTED_EQUALSIGN:
        messageNode(doc, TidyWarning, node, fmt, tagdesc);
        break;

    case XML_ID_SYNTAX:
    case PROPRIETARY_ATTR_VALUE:
    case ANCHOR_NOT_UNIQUE:
    case ATTR_VALUE_NOT_LCASE:
        messageNode(doc, TidyWarning, node, fmt, tagdesc, value);
        break;


    case MISSING_IMAGEMAP:  /* this is not used anywhere */
        messageNode(doc, TidyWarning, node, fmt, tagdesc);
        doc->badAccess |= MISSING_IMAGE_MAP;
        break;

    case REPEATED_ATTRIBUTE:
        messageNode(doc, TidyWarning, node, fmt, tagdesc, value, name);
        break;

    case UNEXPECTED_END_OF_FILE_ATTR:
        /* on end of file adjust reported position to end of input */
        doc->lexer->lines   = doc->docIn->curline;
        doc->lexer->columns = doc->docIn->curcol;
        messageLexer(doc, TidyWarning, fmt, tagdesc);
        break;
    }
}

void ReportMissingAttr( TidyDocImpl* doc, Node* node, ctmbstr name )
{
    /* ReportAttrError( doc, node, NULL, MISSING_ATTRIBUTE ); */
    char tagdesc[ 64 ];
    TagToString(node, tagdesc, sizeof(tagdesc));
    messageNode( doc, TidyWarning, node,
                 "%s lacks \"%s\" attribute", tagdesc, name );
}

#if SUPPORT_ACCESSIBILITY_CHECKS

/*********************************************************
* Accessibility
*
* DisplayHTMLTableAlgorithm()
*
* If the table does contain 2 or more logical levels of 
* row or column headers, the HTML 4 table algorithm 
* to show the author how the headers are currently associated 
* with the cells.
*********************************************************/
 
void DisplayHTMLTableAlgorithm( TidyDocImpl* doc )
{
    tidy_out(doc, " \n");
    tidy_out(doc, "      - First, search left from the cell's position to find row header cells.\n");
    tidy_out(doc, "      - Then search upwards to find column header cells.\n");
    tidy_out(doc, "      - The search in a given direction stops when the edge of the table is\n");
    tidy_out(doc, "        reached or when a data cell is found after a header cell.\n"); 
    tidy_out(doc, "      - Row headers are inserted into the list in the order they appear in\n");
    tidy_out(doc, "        the table. \n");
    tidy_out(doc, "      - For left-to-right tables, headers are inserted from left to right.\n");
    tidy_out(doc, "      - Column headers are inserted after row headers, in \n");
    tidy_out(doc, "        the order they appear in the table, from top to bottom. \n");
    tidy_out(doc, "      - If a header cell has the headers attribute set, then the headers \n");
    tidy_out(doc, "        referenced by this attribute are inserted into the list and the \n");
    tidy_out(doc, "        search stops for the current direction.\n");
    tidy_out(doc, "        TD cells that set the axis attribute are also treated as header cells.\n");
    tidy_out(doc, " \n");
}

void ReportAccessWarning( TidyDocImpl* doc, Node* node, uint code )
{
    ctmbstr fmt = GetFormatFromCode(code);
    doc->badAccess = yes;
    messageNode( doc, TidyAccess, node, fmt );
}

void ReportAccessError( TidyDocImpl* doc, Node* node, uint code )
{
    ctmbstr fmt = GetFormatFromCode(code);
    doc->badAccess = yes;
    messageNode( doc, TidyAccess, node, fmt );
}

#endif /* SUPPORT_ACCESSIBILITY_CHECKS */

void ReportWarning(TidyDocImpl* doc, Node *element, Node *node, uint code)
{
    Node* rpt = (element ? element : node);
    ctmbstr fmt = GetFormatFromCode(code);
    char nodedesc[256] = { 0 };
    char elemdesc[256] = { 0 };

    assert( fmt != NULL );

    TagToString(node, nodedesc, sizeof(nodedesc));

    switch (code)
    {
    case NESTED_QUOTATION:
        messageNode(doc, TidyWarning, rpt, fmt);
        break;

    case OBSOLETE_ELEMENT:
        TagToString(element, elemdesc, sizeof(elemdesc));
        messageNode(doc, TidyWarning, rpt, fmt, elemdesc, nodedesc);
        break;

    case NESTED_EMPHASIS:
        messageNode(doc, TidyWarning, rpt, fmt, nodedesc);
        break;
    case COERCE_TO_ENDTAG_WARN:
        messageNode(doc, TidyWarning, rpt, fmt, node->element, node->element);
        break;
    }
}

void ReportNotice(TidyDocImpl* doc, Node *element, Node *node, uint code)
{
    Node* rpt = ( element ? element : node );
    ctmbstr fmt = GetFormatFromCode(code);
    char nodedesc[256] = { 0 };
    char elemdesc[256] = { 0 };

    assert( fmt != NULL );

    TagToString(node, nodedesc, sizeof(nodedesc));

    switch (code)
    {
    case TRIM_EMPTY_ELEMENT:
        TagToString(element, elemdesc, sizeof(nodedesc));
        messageNode(doc, TidyWarning, element, fmt, elemdesc);
        break;

    case REPLACING_ELEMENT:
        TagToString(element, elemdesc, sizeof(elemdesc));
        messageNode(doc, TidyWarning, rpt, fmt, elemdesc, nodedesc);
        break;
    }
}

void ReportError(TidyDocImpl* doc, Node *element, Node *node, uint code)
{
    char nodedesc[ 256 ] = {0};
    char elemdesc[ 256 ] = {0};
    Node* rpt = ( element ? element : node );
    ctmbstr fmt = GetFormatFromCode(code);

    assert( fmt != NULL );

    TagToString(node, nodedesc, sizeof(nodedesc));

    switch ( code )
    {
    case MISSING_STARTTAG:
    case UNEXPECTED_ENDTAG:
    case TOO_MANY_ELEMENTS:
    case INSERTING_TAG:
        messageNode(doc, TidyWarning, node, fmt, node->element);
        break;

    case USING_BR_INPLACE_OF:
    case CANT_BE_NESTED:
    case PROPRIETARY_ELEMENT:
    case UNESCAPED_ELEMENT:
    case NOFRAMES_CONTENT:
        messageNode(doc, TidyWarning, node, fmt, nodedesc);
        break;

    case MISSING_TITLE_ELEMENT:
    case INCONSISTENT_VERSION:
    case MALFORMED_DOCTYPE:
    case CONTENT_AFTER_BODY:
    case MALFORMED_COMMENT:
    case BAD_COMMENT_CHARS:
    case BAD_XML_COMMENT:
    case BAD_CDATA_CONTENT:
    case INCONSISTENT_NAMESPACE:
    case DOCTYPE_AFTER_TAGS:
    case DTYPE_NOT_UPPER_CASE:
        messageNode(doc, TidyWarning, rpt, fmt);
        break;

    case COERCE_TO_ENDTAG:
    case NON_MATCHING_ENDTAG:
        messageNode(doc, TidyWarning, rpt, fmt, node->element, node->element);
        break;

    case UNEXPECTED_ENDTAG_IN:
    case TOO_MANY_ELEMENTS_IN:
        messageNode(doc, TidyWarning, node, fmt, node->element, element->element);
        break;

    case ENCODING_IO_CONFLICT:
    case MISSING_DOCTYPE:
    case SPACE_PRECEDING_XMLDECL:
        messageNode(doc, TidyWarning, node, fmt);
        break;

    case TRIM_EMPTY_ELEMENT:
    case ILLEGAL_NESTING:
    case UNEXPECTED_END_OF_FILE:
    case ELEMENT_NOT_EMPTY:
        TagToString(element, elemdesc, sizeof(elemdesc));
        messageNode(doc, TidyWarning, element, fmt, elemdesc);
        break;


    case MISSING_ENDTAG_FOR:
        messageNode(doc, TidyWarning, rpt, fmt, element->element);
        break;

    case MISSING_ENDTAG_BEFORE:
        messageNode(doc, TidyWarning, rpt, fmt, element->element, nodedesc);
        break;

    case DISCARDING_UNEXPECTED:
        /* Force error if in a bad form */
        messageNode(doc, doc->badForm ? TidyError : TidyWarning, node, fmt, nodedesc);
        break;

    case TAG_NOT_ALLOWED_IN:
        messageNode(doc, TidyWarning, rpt, fmt, nodedesc, element->element);
        break;

    case REPLACING_UNEX_ELEMENT:
        TagToString(element, elemdesc, sizeof(elemdesc));
        messageNode(doc, TidyWarning, rpt, fmt, elemdesc, nodedesc);
        break;
    }
}

void ReportFatal( TidyDocImpl* doc, Node *element, Node *node, uint code)
{
    char nodedesc[ 256 ] = {0};
    Node* rpt = ( element ? element : node );
    ctmbstr fmt = GetFormatFromCode(code);

    switch ( code )
    {
    case SUSPECTED_MISSING_QUOTE:
    case DUPLICATE_FRAMESET:
        messageNode(doc, TidyError, rpt, fmt);
        break;

    case UNKNOWN_ELEMENT:
        TagToString(node, nodedesc, sizeof(nodedesc));
        messageNode( doc, TidyError, node, fmt, nodedesc );
        break;

    case UNEXPECTED_ENDTAG_IN:
        messageNode(doc, TidyError, node, fmt, node->element, element->element);
        break;

    case UNEXPECTED_ENDTAG:  /* generated by XML docs */
        messageNode(doc, TidyError, node, fmt, node->element);
        break;
    }
}

void ErrorSummary( TidyDocImpl* doc )
{
    /* adjust badAccess to that its NULL if frames are ok */
    ctmbstr encnam = "specified";
    int charenc = cfg( doc, TidyCharEncoding ); 
    if ( charenc == WIN1252 ) 
        encnam = "Windows-1252";
    else if ( charenc == MACROMAN )
        encnam = "MacRoman";
    else if ( charenc == IBM858 )
        encnam = "ibm858";
    else if ( charenc == LATIN0 )
        encnam = "latin0";

    if ( doc->badAccess & (USING_FRAMES | USING_NOFRAMES) )
    {
        if (!((doc->badAccess & USING_FRAMES) && !(doc->badAccess & USING_NOFRAMES)))
            doc->badAccess &= ~(USING_FRAMES | USING_NOFRAMES);
    }

    if (doc->badChars)
    {
#if 0
        if ( doc->badChars & WINDOWS_CHARS )
        {
            tidy_out(doc, "Characters codes for the Microsoft Windows fonts in the range\n");
            tidy_out(doc, "128 - 159 may not be recognized on other platforms. You are\n");
            tidy_out(doc, "instead recommended to use named entities, e.g. &trade; rather\n");
            tidy_out(doc, "than Windows character code 153 (0x2122 in Unicode). Note that\n");
            tidy_out(doc, "as of February 1998 few browsers support the new entities.\n\n");
        }
#endif
        if (doc->badChars & BC_VENDOR_SPECIFIC_CHARS)
        {

            tidy_out(doc, "It is unlikely that vendor-specific, system-dependent encodings\n");
            tidy_out(doc, "work widely enough on the World Wide Web; you should avoid using the \n");
            tidy_out(doc, "%s", encnam );
            tidy_out(doc, " character encoding, instead you are recommended to\n" );
            tidy_out(doc, "use named entities, e.g. &trade;.\n\n");
        }
        if ((doc->badChars & BC_INVALID_SGML_CHARS) || (doc->badChars & BC_INVALID_NCR))
        {
            tidy_out(doc, "Character codes 128 to 159 (U+0080 to U+009F) are not allowed in HTML;\n");
            tidy_out(doc, "even if they were, they would likely be unprintable control characters.\n");
            tidy_out(doc, "Tidy assumed you wanted to refer to a character with the same byte value in the \n");
            tidy_out(doc, "%s", encnam );
            tidy_out(doc, " encoding and replaced that reference with the Unicode equivalent.\n\n" );
        }
        if (doc->badChars & BC_INVALID_UTF8)
        {
            tidy_out(doc, "Character codes for UTF-8 must be in the range: U+0000 to U+10FFFF.\n");
            tidy_out(doc, "The definition of UTF-8 in Annex D of ISO/IEC 10646-1:2000 also\n");
            tidy_out(doc, "allows for the use of five- and six-byte sequences to encode\n");
            tidy_out(doc, "characters that are outside the range of the Unicode character set;\n");
            tidy_out(doc, "those five- and six-byte sequences are illegal for the use of\n");
            tidy_out(doc, "UTF-8 as a transformation of Unicode characters. ISO/IEC 10646\n");
            tidy_out(doc, "does not allow mapping of unpaired surrogates, nor U+FFFE and U+FFFF\n");
            tidy_out(doc, "(but it does allow other noncharacters). For more information please refer to\n");
            tidy_out(doc, "http://www.unicode.org/unicode and http://www.cl.cam.ac.uk/~mgk25/unicode.html\n\n");
        }

#if SUPPORT_UTF16_ENCODINGS

      if (doc->badChars & BC_INVALID_UTF16)
      {
        tidy_out(doc, "Character codes for UTF-16 must be in the range: U+0000 to U+10FFFF.\n");
        tidy_out(doc, "The definition of UTF-16 in Annex C of ISO/IEC 10646-1:2000 does not allow the\n");
        tidy_out(doc, "mapping of unpaired surrogates. For more information please refer to\n");
        tidy_out(doc, "http://www.unicode.org/unicode and http://www.cl.cam.ac.uk/~mgk25/unicode.html\n\n");
      }

#endif

      if (doc->badChars & BC_INVALID_URI)
      {
        tidy_out(doc, "URIs must be properly escaped, they must not contain unescaped\n");
        tidy_out(doc, "characters below U+0021 including the space character and not\n");
        tidy_out(doc, "above U+007E. Tidy escapes the URI for you as recommended by\n");
        tidy_out(doc, "HTML 4.01 section B.2.1 and XML 1.0 section 4.2.2. Some user agents\n");
        tidy_out(doc, "use another algorithm to escape such URIs and some server-sided\n");
        tidy_out(doc, "scripts depend on that. If you want to depend on that, you must\n");
        tidy_out(doc, "escape the URI by your own. For more information please refer to\n");
        tidy_out(doc, "http://www.w3.org/International/O-URL-and-ident.html\n\n");
      }
    }

    if (doc->badForm)
    {
        tidy_out(doc, "You may need to move one or both of the <form> and </form>\n");
        tidy_out(doc, "tags. HTML elements should be properly nested and form elements\n");
        tidy_out(doc, "are no exception. For instance you should not place the <form>\n");
        tidy_out(doc, "in one table cell and the </form> in another. If the <form> is\n");
        tidy_out(doc, "placed before a table, the </form> cannot be placed inside the\n");
        tidy_out(doc, "table! Note that one form can't be nested inside another!\n\n");
    }
    
    if (doc->badAccess)
    {
      if ( cfg(doc, TidyAccessibilityCheckLevel) > 0 )
      {
        tidy_out(doc, "For further advice on how to make your pages accessible, see\n");
        tidy_out(doc, "%s", ACCESS_URL );
        tidy_out(doc, "and\n" );
        tidy_out(doc, "%s", ATRC_ACCESS_URL );
        tidy_out(doc, ".\n" );
        tidy_out(doc, "You may also want to try \"http://www.cast.org/bobby/\" which is a free Web-based\n");
        tidy_out(doc, "service for checking URLs for accessibility.\n\n");
      }
      else
      {
        if (doc->badAccess & MISSING_SUMMARY)
        {
          tidy_out(doc, "The table summary attribute should be used to describe\n");
          tidy_out(doc, "the table structure. It is very helpful for people using\n");
          tidy_out(doc, "non-visual browsers. The scope and headers attributes for\n");
          tidy_out(doc, "table cells are useful for specifying which headers apply\n");
          tidy_out(doc, "to each table cell, enabling non-visual browsers to provide\n");
          tidy_out(doc, "a meaningful context for each cell.\n\n");
        }

        if (doc->badAccess & MISSING_IMAGE_ALT)
        {
          tidy_out(doc, "The alt attribute should be used to give a short description\n");
          tidy_out(doc, "of an image; longer descriptions should be given with the\n");
          tidy_out(doc, "longdesc attribute which takes a URL linked to the description.\n");
          tidy_out(doc, "These measures are needed for people using non-graphical browsers.\n\n");
        }

        if (doc->badAccess & MISSING_IMAGE_MAP)
        {
          tidy_out(doc, "Use client-side image maps in preference to server-side image\n");
          tidy_out(doc, "maps as the latter are inaccessible to people using non-\n");
          tidy_out(doc, "graphical browsers. In addition, client-side maps are easier\n");
          tidy_out(doc, "to set up and provide immediate feedback to users.\n\n");
        }

        if (doc->badAccess & MISSING_LINK_ALT)
        {
          tidy_out(doc, "For hypertext links defined using a client-side image map, you\n");
          tidy_out(doc, "need to use the alt attribute to provide a textual description\n");
          tidy_out(doc, "of the link for people using non-graphical browsers.\n\n");
        }

        if ((doc->badAccess & USING_FRAMES) && !(doc->badAccess & USING_NOFRAMES))
        {
          tidy_out(doc, "Pages designed using frames presents problems for\n");
          tidy_out(doc, "people who are either blind or using a browser that\n");
          tidy_out(doc, "doesn't support frames. A frames-based page should always\n");
          tidy_out(doc, "include an alternative layout inside a NOFRAMES element.\n\n");
        }

        tidy_out(doc, "For further advice on how to make your pages accessible\n");
        tidy_out(doc, "see " );
        tidy_out(doc, ACCESS_URL );
        tidy_out(doc, ". You may also want to try\n" );
        tidy_out(doc, "\"http://www.cast.org/bobby/\" which is a free Web-based\n");
        tidy_out(doc, "service for checking URLs for accessibility.\n\n");
      }
    }

    if (doc->badLayout)
    {
        if (doc->badLayout & USING_LAYER)
        {
            tidy_out(doc, "The Cascading Style Sheets (CSS) Positioning mechanism\n");
            tidy_out(doc, "is recommended in preference to the proprietary <LAYER>\n");
            tidy_out(doc, "element due to limited vendor support for LAYER.\n\n");
        }

        if (doc->badLayout & USING_SPACER)
        {
            tidy_out(doc, "You are recommended to use CSS for controlling white\n");
            tidy_out(doc, "space (e.g. for indentation, margins and line spacing).\n");
            tidy_out(doc, "The proprietary <SPACER> element has limited vendor support.\n\n");
        }

        if (doc->badLayout & USING_FONT)
        {
            tidy_out(doc, "You are recommended to use CSS to specify the font and\n");
            tidy_out(doc, "properties such as its size and color. This will reduce\n");
            tidy_out(doc, "the size of HTML files and make them easier to maintain\n");
            tidy_out(doc, "compared with using <FONT> elements.\n\n");
        }

        if (doc->badLayout & USING_NOBR)
        {
            tidy_out(doc, "You are recommended to use CSS to control line wrapping.\n");
            tidy_out(doc, "Use \"white-space: nowrap\" to inhibit wrapping in place\n");
            tidy_out(doc, "of inserting <NOBR>...</NOBR> into the markup.\n\n");
        }

        if (doc->badLayout & USING_BODY)
        {
            tidy_out(doc, "You are recommended to use CSS to specify page and link colors\n");
        }
    }
}

void UnknownOption( TidyDocImpl* doc, char c )
{
    message( doc, TidyConfig,
             "unrecognized option -%c use -help to list options\n", c );
}

void UnknownFile( TidyDocImpl* doc, ctmbstr program, ctmbstr file )
{
    message( doc, TidyConfig, 
             "%s: can't open file \"%s\"\n", program, file );
}

void NeedsAuthorIntervention( TidyDocImpl* doc )
{
    tidy_out(doc, "This document has errors that must be fixed before\n");
    tidy_out(doc, "using HTML Tidy to generate a tidied up version.\n\n");
}

void GeneralInfo( TidyDocImpl* doc )
{
    tidy_out(doc, "To learn more about HTML Tidy see http://tidy.sourceforge.net\n");
    tidy_out(doc, "Please send bug reports to html-tidy@w3.org\n");
    tidy_out(doc, "HTML and CSS specifications are available from http://www.w3.org/\n");
    tidy_out(doc, "Lobby your company to join W3C, see http://www.w3.org/Consortium\n");
}

#if SUPPORT_ACCESSIBILITY_CHECKS

void AccessibilityHelloMessage( TidyDocImpl* doc )
{
    tidy_out( doc, "\n" );
    tidy_out( doc, "Accessibility Checks: Version 0.1\n" );
    tidy_out( doc, "\n" );
}

#endif /* SUPPORT_ACCESSIBILITY_CHECKS */

void HelloMessage( TidyDocImpl* doc, ctmbstr date, ctmbstr filename )
{
    tmbchar buf[ 2048 ];
    ctmbstr platform = "", helper = "";
    ctmbstr msgfmt = "\nHTML Tidy for %s (vers %s; built on %s, at %s)\n"
                  "Parsing \"%s\"\n";

#ifdef PLATFORM_NAME
    platform = PLATFORM_NAME;
    helper = " for ";
#endif
    
    if ( tmbstrcmp(filename, "stdin") == 0 )
    {
        /* Filename will be ignored at end of varargs */
        msgfmt = "\nHTML Tidy for %s (vers %s; built on %s, at %s)\n"
                 "Parsing console input (stdin)\n";
    }
    
    tmbsnprintf(buf, sizeof(buf), msgfmt, helper, platform, 
             date, __DATE__, __TIME__, filename);
    tidy_out( doc, buf );
}

void ReportMarkupVersion( TidyDocImpl* doc )
{
    if (doc->givenDoctype)
    {
        /* todo: deal with non-ASCII characters in FPI */
        message(doc, TidyInfo, "Doctype given is \"%s\"", doc->givenDoctype);
    }

    if ( ! cfgBool(doc, TidyXmlTags) )
    {
        Bool isXhtml = doc->lexer->isvoyager;
        uint apparentVers;
        ctmbstr vers;

        if ((doc->lexer->doctype == XH11 || 
             doc->lexer->doctype == XB10) &&
            (doc->lexer->versions & doc->lexer->doctype))
            apparentVers = doc->lexer->doctype;
        else
            apparentVers = HTMLVersion(doc);

        vers = HTMLVersionNameFromCode( apparentVers, isXhtml );

        if (!vers)
            vers = "HTML Proprietary";

        message( doc, TidyInfo, "Document content looks like %s", vers );
    }
}

void ReportNumWarnings( TidyDocImpl* doc )
{
    if ( doc->warnings > 0 || doc->errors > 0 )
    {
        tidy_out( doc, "%d %s, %d %s were found!",
                  doc->warnings, doc->warnings == 1 ? "warning" : "warnings",
                  doc->errors, doc->errors == 1 ? "error" : "errors" );

        if ( doc->errors > cfg(doc, TidyShowErrors) ||
             !cfgBool(doc, TidyShowWarnings) )
            tidy_out( doc, " Not all warnings/errors were shown.\n\n" );
        else
            tidy_out( doc, "\n\n" );
    }
    else
        tidy_out( doc, "No warnings or errors were found.\n\n" );
}
