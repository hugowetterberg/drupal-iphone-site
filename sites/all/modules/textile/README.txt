textile.module
README.txt
$Id: README.txt,v 1.5 2007/06/28 00:26:09 joshk Exp $

The Textile module allows you to enter content using Textile, a
simple, plain text syntax that is filtered into valid XHTML. The
filter tips found on the filter/tips page of your Drupal site provides
syntax descriptions and examples. If you are using the title module,
you will need to ensure that Textile comes before the title module on
the filter ordering page.

Textile can be enabled on a per-input-format basis.  There is an
option on the configuration page of each input format for indicating
whether or not Textile processing should only occur on text surrounded
by [textile] and (optional) [/textile] tags.  If an input format is
designed or required to use Textile, this option can be disabled, and
all input will be filtered.

Files
  - textile.module
      the actual module (PHP source code)

  - textile.info
      the module information file used by Drupal

  - classTextile.php (external dependency)
      you will need to download the most up-to-date textile library from:
      
      http://textile.thresholdstate.com/
      
      and place its classTetxile.php file in to the same directory as 
      textile.module

  - README.txt (this file)
      general module information

  - INSTALL.txt
      installation/configuration instructions

  - CREDITS.txt
      information on those responsible for this module

  - TODO.txt
      feature requests and modification suggestions

  - CHANGELOG.txt
      change/release history for this module

  - LICENSE.txt
      the license (GNU General Public License) covering the usage,
      modification, and distribution of this software and its
      accompanying files except for SmartyPants-PHP.inc which is
      covered under a custom redistribution license
