<?php
/**
 * @file
 * Implements IPVenger non-admin pages in Drupal.
 */

use IPVenger\CMS\IPVengerDrupalAppeal;
use IPVenger\CMS\IPVengerDrupalCache;
use IPVenger\Securimage\IPVengerSecurimage;
use IPVenger\Service\IPVengerSettings;
use IPVenger\Service\IPVengerServiceFactory;

/**
 * Block page callback.
 */
function ipvenger_block_page() {
  $library = libraries_load('ipvenger');
  if ($library['installed']) {
    $ip_address = ip_address();
    $settings = ipvenger_settings();

    $service = IPVengerServiceFactory::get();
    $control_center_url = url('admin/config/security/ipvenger/control-center', array('absolute' => TRUE));
    if ($_SERVER['REQUEST_METHOD'] == 'POST') {
      $data = $service->getRiskResponse($settings, new IPVengerDrupalCache(), $ip_address);
      $validated = $service->processFormSubmit($data, new IPVengerDrupalAppeal(), $settings->getAppealEmail(), $control_center_url);
      if ($validated) {
        drupal_goto();
      }
      else {
        drupal_goto(current_path());
      }
    }
    else {
      $data = $service->getRiskResponse($settings, new IPVengerDrupalCache(), $ip_address);
      $service->printErrorMessage($settings, $data, base_path());
    }
  }
}

/**
 * Captcha image image.
 *
 * Returns a captcha image.
 */
function ipvenger_captcha_image() {
  $library = libraries_load('securimage');
  if ($library['installed']) {
    $ip_address = ip_address();

    $secure_image = new IPVengerSecurimage();
    $secure_image->getNextCaptcha($ip_address);
  }
}
