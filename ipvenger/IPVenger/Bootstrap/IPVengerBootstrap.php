<?php
/**
 * @file
 * Convenience file that bootstraps autoloading for IPVenger.
 */

// Constants are slow, so we use as few as possible.
if (!defined('IPVENGER_PREFIX')) {
  define('IPVENGER_PREFIX', realpath(dirname(__FILE__) . DIRECTORY_SEPARATOR . '..' . DIRECTORY_SEPARATOR . '..'));
}

/**
 * Bootstrap class that contains meta-functionality for IPVenger such as
 * the autoload function.
 *
 * @note
 *      This class may be used without any other files from IPVenger.
 */
class IPVengerBootstrap {
  /**
   * Autoload function for IPVenger
   * @param $class
   *   Class to load
   */
  public static function autoload($class) {
    $file = IPVengerBootstrap::getPath($class);
    if (!$file) return false;
    // Technically speaking, it should be ok and more efficient to
    // just do 'require', but Antonio Parraga reports that with
    // Zend extensions such as Zend debugger and APC, this invariant
    // may be broken.  Since we have efficient alternatives, pay
    // the cost here and avoid the bug.
    require_once IPVENGER_PREFIX . DIRECTORY_SEPARATOR . $file;
    return true;
  }

  /**
   * Returns the path for a specific class.
   */
  public static function getPath($class) {
    if (strncmp('IPVenger', $class, 8) !== 0) return false;
    $file = str_replace(array("_", '\\'), '/', $class) . '.php';
    if (!is_readable(IPVENGER_PREFIX . DIRECTORY_SEPARATOR . $file)) return false;
    return $file;
  }

  /**
   * "Pre-registers" our autoloader on the SPL stack.
   */
  public static function registerAutoload() {
    $autoload = array(
      'IPVengerBootstrap',
      'autoload'
    );
    if (($funcs = spl_autoload_functions()) === false) {
      spl_autoload_register($autoload);
    }
    elseif (function_exists('spl_autoload_unregister')) {
      $buggy = version_compare(PHP_VERSION, '5.2.11', '<');
      $compat = version_compare(PHP_VERSION, '5.1.2', '<=') && version_compare(PHP_VERSION, '5.1.0', '>=');
      foreach ($funcs as $func) {
        if ($buggy && is_array($func)) {
          // :TRICKY: There are some compatibility issues and some
          // places where we need to error out
          $reflector = new ReflectionMethod($func[0], $func[1]);
          if (!$reflector->isStatic()) {
            throw new Exception(
              'IPVenger autoloader registrar is not compatible
              with non-static object methods due to PHP Bug #44144;
              Please do not use IPVenger.autoload.php (or any
              file that includes this file); instead, place the code:
              spl_autoload_register(array(\'IPVengerBootstrap\', \'autoload\'))
              after your own autoloaders.');
          }
          // Suprisingly, spl_autoload_register supports the
          // Class::staticMethod callback format, although call_user_func doesn't
          if ($compat)
            $func = implode('::', $func);
        }
        spl_autoload_unregister($func);
      }
      spl_autoload_register($autoload);
      foreach ($funcs as $func)
        spl_autoload_register($func);
    }
  }
}
