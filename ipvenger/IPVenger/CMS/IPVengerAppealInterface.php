<?php
/**
 * @file
 * Definition of IPVenger\CMS\IPVengerAppealInterface.
 */

namespace IPVenger\CMS;

/**
 * Defines an interface for appeal implementations that can vary by CMS.
 *
 * All IPVenger appeal implementations have to implement this interface.
 *
 */
interface IPVengerAppealInterface {
  /**
   * Checks if a valid appeal exists for a given IP address.
   *
   * @param string $ipAddress
   *   The ip address of the database entry to return.
   *
   * @return boolean
   *   The database entry or FALSE on failure.
   */
  public function validAppealExists($ipAddress);

  /**
   * Stores an appeal database entry.
   *
   * @param string $ipAddress
   *   The ip address to store.
   * @param string $emailAddress
   *   The email address to store.
   */
  public function set($ipAddress, $emailAddress);

}
