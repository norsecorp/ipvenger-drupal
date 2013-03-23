<?php
/**
 * @file
 * Definition of IPVenger\Service\IPVengerServiceImpl.
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
 * Defines a implementation of service interactions with IPVenger.
 */
final class IPVengerServiceImpl extends IPVengerService {

  public function isApiKeyNotValid($apiKey) {
    $requestData = array('apikey' => $apiKey,
      'method' => 'ipq',
      'ip' => '4.2.2.2',
      'customID' 	=> $_SERVER['SERVER_NAME']
    );
    $request = new IPVengerRequest('http://beta.ipviking.com/api/', 'POST',$requestData);
    $request->setAcceptType('application/json');
    $request->execute();
    $responseInfo = $request->getResponseInfo();

    switch ($responseInfo['http_code']) {
      case 302:
        $reason = FALSE;
        break;

      case 400:
        $reason = 'Bad format';
        break;

      case 401:
        $reason = 'Unauthorized';
        break;

      case 402:
        $reason = 'Expired';
        break;

      default:
        $reason = 'Request failed: '. $responseInfo['http_code'];

        break;

    }

    return $reason;
  }

  public function getRiskResponse(IPVengerSettings $settings, IPVengerCacheInterface $cache, $ipAddress) {
    if (isset($_SESSION['ipvenger_status'])
          && $_SESSION['ipvenger_status'] == 'allow'
          && isset($_SESSION['ipvenger_expire_time'])
          && $_SESSION['ipvenger_expire_time'] < REQUEST_TIME) {
      $data = new IPVengerServiceData();
      $data->setIpAddress($ipAddress);
      $data->setTimestamp(REQUEST_TIME);
      $data->setDispositionReason('Session Allowed');
      $data->setDisposition(1);
    }
    elseif (in_array($ipAddress, $settings->getIpAddressWhitelist())) {
      $data = new IPVengerServiceData();
      $data->setIpAddress($ipAddress);
      $data->setTimestamp(REQUEST_TIME);
      $data->setDispositionReason('IP Whitelist');
      $data->setDisposition(1);
    }
    /* elseif ($cached = $cache->get($ipAddress, TRUE)) {
      $data = $cached->data;
    } */
    else {
      $requestData = array('apikey' => $settings->getApiKey(),
				'method' => 'ipq',
				'ip' => '4.2.2.2',
        'customID' 	=> $_SERVER['SERVER_NAME']
      );
		  $request = new IPVengerRequest('http://beta.ipviking.com/api/', 'POST',$requestData);
		  $request->setAcceptType('application/json');
		  $request->execute();
		  $responseInfo = $request->getResponseInfo();

		  $data = new IPVengerServiceData();
		  $data->setIpAddress($ipAddress);
		  $data->setTimestamp(REQUEST_TIME);

		  switch ($responseInfo['http_code']) {
		    case 302: // Success - process as normal.
		      $json = json_decode($request->getResponseBody());

		      $country = $json->response->geoloc->country;
		      empty($country) || $country == '-' ? $data->setCountry('Unknown') : $data->setCountry($country);
		      $city = $json->response->geoloc->city;
		      empty($city) || $city == '-' ? $data->setCity('Unknown') : $data->setCity($city);
		      $organization = $json->response->geoloc->organization;
		      empty($organization) || $organization == '-' ? $data->setOrganization('Unknown') : $data->setOrganization($organization);
		      $factors = get_object_vars($json->response->factoring[0]);
		      arsort($factors, SORT_NUMERIC);
		      // Primary factor.
		      $factorName = ucwords(str_replace('_', ' ', key(array_slice($factors, 0))));
		      $factorName = $factorName == 'Ipviking Category Factor' ? 'IPViking Category Factor' : $factorName;
		      $data->setFactorName($factorName);
		      $data->setRiskFactor($json->response->risk_factor);
		      $data->setCategoryName(ucwords(str_replace('_', ' ', $json->response->entries[0]->category_name)));

		      // Additional misc data in the response.
		      $entry = $json->response->entries[0];
		      $factors = $json->response->factoring[0];
          $data->setAsnRecordFactor($factors->asn_record_factor);
          $data->setAsnThreatFactor($factors->asn_threat_factor);
          $data->setAutonomousSystemName($json->response->ip_info->autonomous_system_name);
          $data->setAutonomousSystemNumber($json->response->ip_info->autonomous_system_number);
          $data->setBgpDelegationFactor($factors->bgp_delegation_factor);
          $data->setCategoryFactor($entry->category_factor);
          $data->setCategoryId($entry->category_id);
          $data->setCountryCode($json->response->geoloc->country_code);
          $data->setCountryRiskFactor($factors->country_risk_factor);
          $data->setDataAgeFactor($factors->data_age_factor);
          $data->setGeoFilterFactor($factors->ipviking_geofilter_factor);
          $data->setGeoFilterRule($factors->ipviking_geofilter_rule);
          $data->setIanaAllocationFactor($factors->iana_allocation_factor);
          $data->setInternetServiceProvider($json->response->geoloc->internet_service_provider);
          $data->setIpResolveFactor($factors->ip_resolve_factor);
          $data->setLatitude($json->response->geoloc->latitude);
          $data->setLongitude($json->response->geoloc->longtitude);
          $data->setPersonalFactor($factors->ipviking_personal_factor);
          $data->setRegion($json->response->geoloc->region);
          $data->setRegionCode($json->response->geoloc->region_code);
          $data->setRegionRiskFactor($factors->region_risk_factor);
          $data->setRiskColor($json->response->risk_color);
          $data->setRiskDescription($json->response->risk_desc);
          $data->setRiskFactor($json->response->risk_factor);
          $data->setRiskName($json->response->risk_name);
          $data->setSearchVolumeFactor($factors->search_volume_factor);
		      break;

		    case 503:	// IP Viking down for maintenance - allow.
		      $data->setDispositionReason('IPV unavailable');
		      $data->setDisposition(1);
		      break;

		    case 417: // Private IP address range - allow.
		      $data->setDispositionReason('Local network');
		      $data->setDisposition(1);
		      break;

		    case 204: // Request OK but no info on IP - allow.
		      $data->setDispositionReason('No info');
		      $data->setDisposition(1);
		      break;

		    // api key errors - allow
		    case 400:
		    case 401:
		    case 402:
		      $data->setDispositionReason('API Key error '. $responseInfo['http_code']);
		      $data->setDisposition(1);
		      break;

		      // Any other error is an internal error or problem at IPV - allow.
		    default:
		      $data->setDispositionReason('Error '. $responseInfo['http_code']);
		      $data->setDisposition(1);
		      break;

		  }

      if (in_array($data->getCountry(), $settings->getCountryBlacklist())) {
        $data->setDispositionReason('Country Blacklist');
        $data->setDisposition(0);
      }
      elseif (in_array($data->getIpAddress(), $settings->getIpAddressBlacklist())) {
        $data->setDispositionReason('IP Blacklist');
        $data->setDisposition(0);
      }

      $cache->set($ipAddress, $data);
    }

    return $data;
  }
}
