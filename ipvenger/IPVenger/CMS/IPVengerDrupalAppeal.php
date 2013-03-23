<?php
/**
 * @file
 * Definition of IPVenger\CMS\IPVengerDrupalAppeal.
 */

namespace IPVenger\CMS;

use IPVenger\CMS\IPVengerAppealInterface;

/**
 * Defines a default appeal implementation for Drupal.
 */
class IPVengerDrupalAppeal implements IPVengerAppealInterface {

  public function validAppealExists($ipAddress) {
    $return = FALSE;

    // If a non-null appealId exists, then we assume a valid appeal exists.
    $query = db_query('SELECT appealId FROM {ipvenger_appeal} WHERE ipAddress = :ip_address AND appealId IS NOT NULL', array(':ip_address' => $ipAddress));
    $result = $query->fetchField();
    if ($result !== FALSE) {
      $return = TRUE;
    }

    return $return;
  }

  public function set($ipAddress, $emailAddress) {
    $record = array(
      'timestamp' => REQUEST_TIME,
      'ipAddress' => $ipAddress,
      'email' => $emailAddress,
    );
    drupal_write_record('ipvenger_appeal', $record);
  }

}
