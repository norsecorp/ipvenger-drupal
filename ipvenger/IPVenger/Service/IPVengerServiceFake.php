<?php
/**
 * @file
 * Definition of IPVenger\Service\IPVengerServiceFake.
 */

namespace IPVenger\Service;

use IPVenger\CMS\IPVengerAppealInterface;
use IPVenger\CMS\IPVengerCacheInterface;
use IPVenger\CMS\IPVengerCacheFactory;
use IPVenger\Securimage\IPVengerSecurimage;
use IPVenger\Service\IPVengerSettings;
use IPVenger\Service\IPVengerService;
use IPVenger\Service\IPVengerServiceData;
use IPVenger\Service\IPVengerRequest;

/**
 * Defines a stub implementation of service interactions with IPVenger.
 */
final class IPVengerServiceFake extends IPVengerService {

  public function isApiKeyNotValid($apiKey) {
    return FALSE;
  }

  public function getRiskResponse(IPVengerSettings $settings, IPVengerCacheInterface $cache, $ipAddress) {

    // Put these in statics so they don't change within a single page render.
    static $called;
    if (!isset($called)) {
      $countries = array('United States', 'Germany', 'Russia', 'China');
      shuffle($countries);
      $cities = array('London', 'New York City', 'Los Angeles', 'Bejing');
      shuffle($cities);
      $organizations = array('AOL', 'Prodigy', 'CompuServe', 'Bell Telephone');
      shuffle($organizations);
      $factorNames = array('IPViking Category Factor', 'Botnet', 'Bogon Unass', 'Asn Record Factor');
      shuffle($factorNames);
      $called = TRUE;
    }

		$data = new IPVengerServiceData();

		$data->setIpAddress('4.2.2.2');
		$data->setTimestamp(REQUEST_TIME);
    $data->setCountry(array_pop($countries));
    $data->setCity(array_pop($cities));
    $data->setOrganization(array_pop($organizations));
    $data->setFactorName(array_pop($factorNames));
    $data->setRiskFactor(rand(1, 50));
    $data->setCategoryName('Misc Category Name');

    if (in_array($data->getCountry(), $settings->getCountryBlacklist())) {
      $data->setDispositionReason('Country Blacklist');
      $data->setDisposition(0);
    }
    elseif (in_array($data->getIpAddress(), $settings->getIpAddressBlacklist())) {
      $data->setDispositionReason('IP Blacklist');
      $data->setDisposition(0);
    }

    $cache->set($ipAddress, $data);

    return $data;
  }
}
