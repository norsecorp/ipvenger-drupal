<?php
/**
 * @file
 * Convenience file that registers autoload handler for IPVenger.
 * It also does some sanity checks.
 */

if (function_exists('spl_autoload_register') && function_exists('spl_autoload_unregister')) {
    // We need unregister for our pre-registering functionality
    IPVengerBootstrap::registerAutoload();
    if (function_exists('__autoload')) {
        // Be polite and ensure that userland autoload gets retained
        spl_autoload_register('__autoload');
    }
} elseif (!function_exists('__autoload')) {
    function __autoload($class) {
        return IPVengerBootstrap::autoload($class);
    }
}
