$Id: README.txt,v 1.1 2008/10/03 05:43:31 crell Exp $
ABOUT

The autoload module is a utility module.  It allows other modules to leverage
PHP 5's class autoloading capabilities in a unified fashion.  It does, naturally,
require PHP 5.1.2 or later.

You do not need this module unless you are developing a module that makes use of
classes or you are installing a module that depends on this one.

Drupal 7's integrated self-learning code registry is far better than this 
module anyway, so this module will never be ported to Drupal 7.  Drupal 6
is the only version that will be supported, but the code conventions used by
this module will make moving to Drupal 7 easier for class-using modules.


API

PHP 5 supports automatic class lazy-loading in user-defined ways.  A developer
can register one or more autoload functions.  When PHP encounters a request for
a class that is not yet loaded, it will then call all registered autoload 
functions to allow them to try and load the library in which that class is 
defined.  It then tries to load the class again.  If the class is now available,
it loads normally.  If not, it generates a fatal error just as it would have
if there had been no autoload functions.  The catch, of course, is making sure
the autoload functions know how to find the requested class.

This module provides two hooks, hook_autoload_info() and hook_autoload_info_alter().  
Both are extremely simple.

hook_autoload_info() is a registry-style hook.  That is, it must return an
associative array, in this case of classes and the file in which they may be 
found.  Example:

/**
 * Implementation of hook_autoload_info().
 */
function example_autoload_info() {
  return array(
    'ExampleClass' => array(
      'file' => 'example.classes.inc',
    ),
    'ExampleInterface' => array(
      'file' => 'example.classes.inc',
    ),
  );
}

The keys of the returned array are the names of classes or interfaces.  Each 
entry has two keys, "file" and "file path", with "file path" being optional.  
They behave exactly like the "file" and "file path" keys behave in the menu
system.  If "file path" is not specified, it defaults to the directory of the
module.  "file" specifies in what file, relative to "file path", the class
is defined.

hook_autoload_info_alter() is a typical alter hook, and is provided primarily
because it's just so easy to provide an alter for a registry-style hook it 
seemed wasteful not to.

That's it!  Once you declare a class and its location (and clear the cache!), 
the autoload module knows about it and will lazy-load the class for you as 
needed.  You do not need to make any other changes to your object-using code.
Simply call:

$example = new ExampleClass();

as you normally would anyway and the rest happens automatically.

The included "autoloadtest" module provides a trivial but working example.  It
does not include an info file in order to keep it from showing up in the modules
list.


OPTIMIZATION

Do not fall into the easy trap of having one class per file!  Lazy-loading classes
is intended to reduce the amount of code that needs to be parsed on every page,
thus improving performance.  However, loading multiple files into memory is slower
than loading a single file into memory.  There is a trade-off that you will need
to optimize for your specific use case.  

In general, classes that will be needed very frequently should remain in the
.module file.  Other classes should be logically grouped into separate include
files so that classes and interfaces that are used together are loaded 
together as well.  That skips the autoloading process as well as the extra disk
hit.

When in doubt, benchmarks are your friend.


CREDITS

This module was developed by Larry "Crell" Garfield
