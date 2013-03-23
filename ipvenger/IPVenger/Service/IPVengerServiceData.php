<?php
/**
 * @file
 * Definition of IPVenger\Serice\IPVengerServiceData.
 */

namespace IPVenger\Service;

final class IPVengerServiceData {

  private $data = array();

  /**
   * @return string
   *   IP submitted to IPViking.
   */
  public function getIpAddress() {
    return isset($this->data['ipAddress']) ? $this->data['ipAddress'] : FALSE;
  }

  /**
   * @param string $ipAddress
   *   IP submitted to IPViking.
   */
  public function setIpAddress($ipAddress) {
    $this->data['ipAddress'] = $ipAddress;
  }

  /**
   * @return int
   *   IPQ Score.
   */
  public function getRiskFactor() {
    return isset($this->data['riskFactor']) ? $this->data['riskFactor'] : FALSE;
  }

  /**
   * @param int $riskFactor
   *   IPQ Score.
   */
  public function setRiskFactor($riskFactor) {
    $this->data['riskFactor'] = $riskFactor;
  }

  /**
   * @return string
   *   Color associated with a IPViking-defined scale of risk.
   */
  public function getRiskColor() {
    return isset($this->data['riskColor']) ? $this->data['riskColor'] : FALSE;
  }

  /**
   * @param string $riskColor
   *   Color associated with a IPViking-defined scale of risk.
   */
  public function setRiskColor($riskColor) {
    $this->data['riskColor'] = $riskColor;
  }

  /**
   * @return string
   *   IPViking-defined general category of risk of Low, Medium, High.
   */
  public function getRiskName() {
    return isset($this->data['riskName']) ? $this->data['riskName'] : FALSE;
  }

  /**
   * @param string $riskName
   *   IPViking-defined general category of risk of Low, Medium, High.
   */
  public function setRiskName($riskName) {
    $this->data['riskName'] = $riskName;
  }

  /**
   * @return string
   *   IPViking-defined general category of risk of Low, Medium, High.
   */
  public function getRiskDescription() {
    return isset($this->data['riskDescription']) ? $this->data['riskDescription'] : FALSE;
  }

  /**
   * @param string $riskDescription
   *   IPViking-defined general category of risk of Low, Medium, High.
   */
  public function setRiskDescription($riskDescription) {
    $this->data['riskDescription'] = $riskDescription;
  }

  /**
   * @return int
   *   The UNIX timestamp of the request.
   */
  public function getTimestamp() {
    return isset($this->data['timestamp']) ? $this->data['timestamp'] : FALSE;
  }

  /**
   * @param int $timestamp
   *   The UNIX timestamp of the request.
   */
  public function setTimestamp($timestamp) {
    $this->data['timestamp'] = $timestamp;
  }

  /**
   * @return string
   *   Autonomous system number for the IP address.
   */
  public function getAutonomousSystemNumber() {
    return isset($this->data['autonomousSystemNumber']) ? $this->data['autonomousSystemNumber'] : FALSE;
  }

  /**
   * @param string $autonomousSystemNumber
   *   Autonomous system number for the IP address.
   */
  public function setAutonomousSystemNumber($autonomousSystemNumber) {
    $this->data['autonomousSystemNumber'] = $autonomousSystemNumber;
  }

  /**
   * @return string
   *   Autonomous system name for the IP address.
   */
  public function getAutonomousSystemName() {
    return isset($this->data['autonomousSystemName']) ? $this->data['autonomousSystemName'] : FALSE;
  }

  /**
   * @param string $autonomousSystemName
   *   Autonomous system name for the IP address.
   */
  public function setAutonomousSystemName($autonomousSystemName) {
    $this->data['autonomousSystemName'] = $autonomousSystemName;
  }

  /**
   * @return string
   *   Country name.
   */
  public function getCountry() {
    return isset($this->data['country']) ? $this->data['country'] : FALSE;
  }

  /**
   * @param string $country
   *   Country name.
   */
  public function setCountry($country) {
    $this->data['country'] = $country;
  }

  /**
   * @return string
   *   Country code per the ISO Standard.
   */
  public function getCountryCode() {
    return isset($this->data['countryCode']) ? $this->data['countryCode'] : FALSE;
  }

  /**
   * @param string $countryCode
   *   Country code per the ISO Standard.
   */
  public function setCountryCode($countryCode) {
    $this->data['countryCode'] = $countryCode;
  }

  /**
   * @return string
   *   The state or region name.
   */
  public function getRegion() {
    return isset($this->data['region']) ? $this->data['region'] : FALSE;
  }

  /**
   * @param string $region
   *   The state or region name.
   */
  public function setRegion($region) {
    $this->data['region'] = $region;
  }

  /**
   * @return string
   *   The Region/Province/Territory code per the ISO Standard.
   */
  public function getRegionCode() {
    return isset($this->data['regionCode']) ? $this->data['regionCode'] : FALSE;
  }

  /**
   * @param string $regionCode
   *   The Region/Province/Territory code per the ISO Standard.
   */
  public function setRegionCode($regionCode) {
    $this->data['regionCode'] = $regionCode;
  }

  /**
   * @return string
   *   City name.
   */
  public function getCity() {
    return isset($this->data['city']) ? $this->data['city'] : FALSE;
  }

  /**
   * @param string $city
   *   City name.
   */
  public function setCity($city) {
    $this->data['city'] = $city;
  }

  /**
   * @return float
   *   Latitude of the provide IP address.
   */
  public function getLatitude() {
    return isset($this->data['latitude']) ? $this->data['latitude'] : FALSE;
  }

  /**
   * @param float $latitude
   *   Latitude of the provide IP address.
   */
  public function setLatitude($latitude) {
    $this->data['latitude'] = $latitude;
  }

  /**
   * @return float
   *   Longitude of the provide IP address.
   */
  public function getLongitude() {
    return isset($this->data['longitude']) ? $this->data['longitude'] : FALSE;
  }

  /**
   * @param float $longitude
   *   Longitude of the provide IP address.
   */
  public function setLongitude($longitude) {
    $this->data['longitude'] = $longitude;
  }

  /**
   * @return string
   *   The organization associated with the provided IP.
   */
  public function getInternetServiceProvider() {
    return isset($this->data['internetServiceProvider']) ? $this->data['internetServiceProvider'] : FALSE;
  }

  /**
   * @param string $internetServiceProvider
   *   The organization associated with the provided IP.
   */
  public function setInternetServiceProvider($internetServiceProvider) {
    $this->data['internetServiceProvider'] = $internetServiceProvider;
  }

  /**
   * @return string
   *   The organization associated with the provided IP.
   */
  public function getOrganization() {
    return isset($this->data['organization']) ? $this->data['organization'] : FALSE;
  }

  /**
   * @param string $organization
   *   The organization associated with the provided IP.
   */
  public function setOrganization($organization) {
    $this->data['organization'] = $organization;
  }

  /**
   * @return string
   *   The organization associated with the provided IP.
   */
  public function getCountryRiskFactor() {
    return isset($this->data['countryRiskFactor']) ? $this->data['countryRiskFactor'] : FALSE;
  }

  /**
   * @param float $countryRiskFactor
   *   Statistical analysis reflecting the country risk factor assessment.
   */
  public function setCountryRiskFactor($countryRiskFactor) {
    $this->data['countryRiskFactor'] = $countryRiskFactor;
  }

  /**
   * @return float
   *   Statistical analysis reflecting the region risk factor assessment.
   */
  public function getRegionRiskFactor() {
    return isset($this->data['regionRiskFactor']) ? $this->data['regionRiskFactor'] : FALSE;
  }

  /**
   * @param float $regionRiskFactor
   *   Statistical analysis reflecting the region risk factor assessment.
   */
  public function setRegionRiskFactor($regionRiskFactor) {
    $this->data['regionRiskFactor'] = $regionRiskFactor;
  }

  /**
   * @return float
   *   Factor assessment of the historical and current DNS reverse.
   */
  public function getIpResolveFactor() {
    return isset($this->data['ipResolveFactor']) ? $this->data['ipResolveFactor'] : FALSE;
  }

  /**
   * @param float $ipResolveFactor
   *   Factor assessment of the historical and current DNS reverse.
   */
  public function setIpResolveFactor($ipResolveFactor) {
    $this->data['ipResolveFactor'] = $ipResolveFactor;
  }

  /**
   * @return float
   *   Factor assessment based on the AS (Autonomous system).
   */
  public function getAsnRecordFactor() {
    return isset($this->data['asnRecordFactor']) ? $this->data['asnRecordFactor'] : FALSE;
  }

  /**
   * @param float $asnRecordFactor
   *   Factor assessment based on the AS (Autonomous system).
   */
  public function setAsnRecordFactor($asnRecordFactor) {
    $this->data['asnRecordFactor'] = $asnRecordFactor;
  }

  /**
   * @return float
   *   Statistical analysis on incidents resolved to ASN assessment.
   */
  public function getAsnThreatFactor() {
    return isset($this->data['asnThreatFactor']) ? $this->data['asnThreatFactor'] : FALSE;
  }

  /**
   * @param float $asnThreatFactor
   *   Statistical analysis on incidents resolved to ASN assessment.
   */
  public function setAsnThreatFactor($asnThreatFactor) {
    $this->data['asnThreatFactor'] = $asnThreatFactor;
  }

  /**
   * @return float
   *   BGP delegation records assessment of multiple characteristics.
   */
  public function getBgpDelegationFactor() {
    return isset($this->data['bgpDelegationFactor']) ? $this->data['bgpDelegationFactor'] : FALSE;
  }

  /**
   * @param float $bgpDelegationFactor
   *   BGP delegation records assessment of multiple characteristics.
   */
  public function setBgpDelegationFactor($bgpDelegationFactor) {
    $this->data['bgpDelegationFactor'] = $bgpDelegationFactor;
  }

  /**
   * @return float
   *   Assessment on IANA allocation records.
   */
  public function getIanaAllocationFactor() {
    return isset($this->data['ianaAllocationFactor']) ? $this->data['ianaAllocationFactor'] : FALSE;
  }

  /**
   * @param float $ianaAllocationFactor
   *   Assessment on IANA allocation records.
   */
  public function setIanaAllocationFactor($ianaAllocationFactor) {
    $this->data['ianaAllocationFactor'] = $ianaAllocationFactor;
  }

  /**
   * @return float
   *   Statistical analysis of user submitted data.
   *
   */
  public function getPersonalFactor() {
    return isset($this->data['personalFactor']) ? $this->data['personalFactor'] : FALSE;
  }

  /**
   * @param float $personalFactor
   *   Statistical analysis of user submitted data.
   */
  public function setPersonalFactor($personalFactor) {
    $this->data['personalFactor'] = $personalFactor;
  }

  /**
   * @return float
   *   Contextual statistical analysis of collected internet intelligence.
   */
  public function getCategoryFactor() {
    return isset($this->data['categoryFactor']) ? $this->data['categoryFactor'] : FALSE;
  }

  /**
   * @param float $categoryFactor
   *   Contextual statistical analysis of collected internet intelligence.
   */
  public function setCategoryFactor($categoryFactor) {
    $this->data['categoryFactor'] = $categoryFactor;
  }

  /**
   * @return float
   *   Factor derived from the user manipulated GeoFiltering system.
   */
  public function getGeoFilterFactor() {
    return isset($this->data['geoFilterFactor']) ? $this->data['geoFilterFactor'] : FALSE;
  }

  /**
   * @param float $geoFilterFactor
   *   Factor derived from the user manipulated GeoFiltering system.
   */
  public function setGeoFilterFactor($geoFilterFactor) {
    $this->data['geoFilterFactor'] = $geoFilterFactor;
  }

  /**
   * @return float
   *   Rule ID from user submitted rules used to derived the factor.
   */
  public function getGeoFilterRule() {
    return isset($this->data['geoFilterRule']) ? $this->data['geoFilterRule'] : FALSE;
  }

  /**
   * @param float $geoFilterRule
   *   Rule ID from user submitted rules used to derived the factor.
   */
  public function setGeoFilterRule($geoFilterRule) {
    $this->data['geoFilterRule'] = $geoFilterRule;
  }

  /**
   * @return float
   *   Factor assessment derived from factor timestamps used in risk assessment.
   */
  public function getDataAgeFactor() {
    return isset($this->data['dataAgeFactor']) ? $this->data['dataAgeFactor'] : FALSE;
  }

  /**
   * @param float $dataAgeFactor
   *   Factor assessment derived from factor timestamps used in risk assessment.
   */
  public function setDataAgeFactor($dataAgeFactor) {
    $this->data['dataAgeFactor'] = $dataAgeFactor;
  }

  /**
   * @return float
   *   Factor assessment based on the frequenzy and volume the IP is searched.
   */
  public function getSearchVolumeFactor() {
    return isset($this->data['searchVolumeFactor']) ? $this->data['searchVolumeFactor'] : FALSE;
  }

  /**
   * @param float $searchVolumeFactor
   *   Factor assessment based on the frequenzy and volume the IP is searched.
   */
  public function setSearchVolumeFactor($searchVolumeFactor) {
    $this->data['searchVolumeFactor'] = $searchVolumeFactor;
  }

  /**
   * @return float
   *   General behavior type for the submitted IP.
   */
  public function getCategoryName() {
    return isset($this->data['categoryName']) ? $this->data['categoryName'] : FALSE;
  }

  /**
   * @param float $categoryName
   *   General behavior type for the submitted IP.
   */
  public function setCategoryName($categoryName) {
    $this->data['categoryName'] = $categoryName;
  }

  /**
   * @return float
   *   Specific categorization ID of the behavior category.
   */
  public function getCategoryId() {
    return isset($this->data['categoryId']) ? $this->data['categoryId'] : FALSE;
  }

  /**
   * @param float $categoryId
   *   Specific categorization ID of the behavior category.
   */
  public function setCategoryId($categoryId) {
    $this->data['categoryId'] = $categoryId;
  }

  /**
   * @return float
   *   Highest contributing risk factor name.
   */
  public function getFactorName() {
    return isset($this->data['factorName']) ? $this->data['factorName'] : FALSE;
  }

  /**
   * @param float $factorName
   *   Highest contributing risk factor name.
   */
  public function setFactorName($factorName) {
    $this->data['factorName'] = $factorName;
  }

  /**
   * The disposition reason of the request for future analysis.
   *
   * @return bool
   *   bool | FALSE (0) is a blocked request and TRUE (1) is an allowed request.
   */
  public function getDisposition() {
    return isset($this->data['disposition']) ? $this->data['disposition'] : FALSE;
  }

  /**
   * The disposition reason of the request for future analysis.
   *
   * @param bool $disposition
   *   bool | FALSE (0) is a blocked request and TRUE (1) is an allowed request.
   */
  public function setDisposition($disposition) {
    $this->data['disposition'] = $disposition;
  }

  /**
   * The disposition reason of the request for future analysis.
   *
   * Blocked requests are always assigned one of three disposition reasons:
   * IPQ Score, IP Blacklist or Country Blacklist.
   *
   * @return string
   *   The disposition reason of the request.
   *
   */
  public function getDispositionReason() {
    return isset($this->data['dispositionReason']) ? $this->data['dispositionReason'] : FALSE;
  }

  /**
   * The disposition reason of the request for future analysis.
   *
   * Blocked requests are always assigned one of three disposition reasons:
   * IPQ Score, IP Blacklist or Country Blacklist.
   *
   * @param string $dispositionReason
   *   The disposition reason of the request.
   */
  public function setDispositionReason($dispositionReason) {
    $this->data['dispositionReason'] = $dispositionReason;
  }

  /**
   * @return array
   *   Full data object as an array.
   */
  public function getData() {
    return $this->data;
  }
}
