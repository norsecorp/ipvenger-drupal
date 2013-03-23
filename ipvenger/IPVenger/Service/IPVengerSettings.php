<?php
/**
 * @file
 * Definition of IPVenger\Service\IPVengerSettings.
 */

namespace IPVenger\Service;
/**
 * All the IPVenger settings needed to perform a risk assessment.
 */
final class IPVengerSettings {

  private $apiKey;
  private $siteFunction;
  private $riskThreshold;
  private $allowAppeal;
  private $appealEmail;
  private $generalMessage;
  private $proxyMessage;
  private $botnetMessage;
  private $ipAddressWhitelist;
  private $ipAddressBlacklist;
  private $countryBlacklist;
  private $siteLogo;
  private $siteFavicon;
  private $siteName;
  private $siteDescription;
  private $stylesheet;

  /**
   * @return
   *   The IPVenger API Key.
   */
  public function getApiKey() {
    return $this->apiKey;
  }

  /**
   * @param String $apiKey
   */
  public function setApiKey($apiKey) {
    $this->apiKey = $apiKey;
  }

  /**
   * @return
   *   The site function i.e. blog, ecommerce, custom, etc.
   */
  public function getSiteFunction() {
    return $this->siteFunction;
  }

  /**
   * @param String $siteFunction
   */
  public function setSiteFunction($siteFunction) {
    $this->siteFunction = $siteFunction;
  }

  /**
   * @return
   *   The IPVenger risk threshold.
   */
  public function getRiskThreshold() {
    return $this->riskThreshold;
  }

  /**
   * @param int $risk_threshold
   */
  public function setRiskThreshold($risk_threshold) {
    $this->riskThreshold = $risk_threshold;
  }

  /**
   * @return
   *   The boolean to allow an appeal.
   */
  public function getAllowAppeal() {
    return $this->allowAppeal;
  }

  /**
   * @param boolean $allowAppeal
   */
  public function setAllowAppeal($allowAppeal) {
    $this->allowAppeal = $allowAppeal;
  }

  /**
   * @return
   *   The email address to send appeals.
   */
  public function getAppealEmail() {
    return $this->appealEmail;
  }

  /**
   * @param String $appealEmail
   */
  public function setAppealEmail($appealEmail) {
    $this->appealEmail = $appealEmail;
  }

  /**
   * @return
   *   The general message to give denied site visitors.
   */
  public function getGeneralMessage() {
    return $this->generalMessage;
  }

  /**
   * @param String $generalMessage
   */
  public function setGeneralMessage($generalMessage) {
    $this->generalMessage = $generalMessage;
  }

  /**
   * @return
   *   The message to give denied site visitors from behind a proxy server.
   */
  public function getProxyMessage() {
    return $this->proxyMessage;
  }

  /**
   * @param String $proxyMessage
   */
  public function setProxyMessage($proxyMessage) {
    $this->proxyMessage = $proxyMessage;
  }

  /**
   * @return
   *   The message to give denied site visitors from a botnet machine.
   */
  public function getBotnetMessage() {
    return $this->botnetMessage;
  }

  /**
   * @param String $botnetMessage
   */
  public function setBotnetMessage($botnetMessage) {
    $this->botnetMessage = $botnetMessage;
  }

  /**
   * @return
   *   An array of IP addresses that should always be allowed.
   */
  public function getIpAddressWhitelist() {
    return $this->ipAddressWhitelist;
  }

  /**
   * @param array $ipAddressWhitelist
   */
  public function setIpAddressWhitelist(array $ipAddressWhitelist) {
    $this->ipAddressWhitelist = $ipAddressWhitelist;
  }

  /**
   * @return
   * An array of IP addresses that should always be denied.
   */
  public function getIpAddressBlacklist() {
    return $this->ipAddressBlacklist;
  }

  /**
   * @param array $ipAddressBlacklist
   */
  public function setIpAddressBlacklist(array $ipAddressBlacklist) {
    $this->ipAddressBlacklist = $ipAddressBlacklist;
  }

  /**
   * @return
   *   An array of countries that should always be denied.
   */
  public function getCountryBlacklist() {
    return $this->countryBlacklist;
  }

  /**
   * @param array $countryBlacklist
   */
  public function setCountryBlacklist(array $countryBlacklist) {
    $this->countryBlacklist = $countryBlacklist;
  }

  /**
   * @return
   *   Absolute path to the site logo.
   */
  public function getSiteLogo() {
    return $this->siteLogo;
  }

  /**
   * @param string $siteLogo
   */
  public function setSiteLogo($siteLogo) {
    $this->siteLogo = $siteLogo;
  }

  /**
   * @return
   *   Absolute path to the site favicon.ico.
   */
  public function getSiteFavicon() {
    return $this->siteFavicon;
  }

  /**
   * @param string $siteFavicon
   */
  public function setSiteFavicon($siteFavicon) {
    $this->siteFavicon = $siteFavicon;
  }

  /**
   * @return
   *   The website name.
   */
  public function getSiteName() {
    return $this->siteName;
  }

  /**
   * @param string $siteName
   */
  public function setSiteName($siteName) {
    $this->siteName = $siteName;
  }

  /**
   * @return
   *   The website description or slogon.
   */
  public function getSiteDescription() {
    return $this->siteDescription;
  }

  /**
   *
   * @param string $siteDescription
   */
  public function setSiteDescription($siteDescription) {
    $this->siteDescription = $siteDescription;
  }

  /**
   * @return
   *   The block page stylesheet.
   */
  public function getStylesheet() {
    return $this->stylesheet;
  }

  /**
   *
   * @param string $stylesheet
   */
  public function setStylesheet($stylesheet) {
    $this->stylesheet = $stylesheet;
  }

  /**
   * @return
   *   An array of countries that can be used for white/blacklisting.
   */
  public static function getCountryList() {
    return array(
      "Afghanistan",
      "Aland Islands",
      "Albania",
      "Algeria",
      "American Samoa",
      "Andorra",
      "Angola",
      "Anguilla",
      "Anonymous Proxy",
      "Antarctica",
      "Antigua and Barbuda",
      "Argentina",
      "Armenia",
      "Aruba",
      "Asia/Pacific Region",
      "Australia",
      "Austria",
      "Azerbaijan",
      "Bahamas",
      "Bahrain",
      "Bangladesh",
      "Barbados",
      "Belarus",
      "Belgium",
      "Belize",
      "Benin",
      "Bermuda",
      "Bhutan",
      "Bolivia",
      "Bosnia and Herzegovina",
      "Botswana",
      "Bouvet Island",
      "Brazil",
      "British Indian Ocean Territory",
      "Brunei Darussalam",
      "Bulgaria",
      "Burkina Faso",
      "Burundi",
      "Cambodia",
      "Cameroon",
      "Canada",
      "Cape Verde",
      "Cayman Islands",
      "Central African Republic",
      "Chad",
      "Chile",
      "China",
      "Christmas Island",
      "Cocos (Keeling) Islands",
      "Colombia",
      "Comoros",
      "Congo",
      "Congo, The Democratic Republic of the",
      "Cook Islands",
      "Costa Rica",
      "Cote d'Ivoire",
      "Croatia",
      "Cuba",
      "Cyprus",
      "Czech Republic",
      "Denmark",
      "Djibouti",
      "Dominica",
      "Dominican Republic",
      "Ecuador",
      "Egypt",
      "El Salvador",
      "Equatorial Guinea",
      "Eritrea",
      "Estonia",
      "Ethiopia",
      "Europe",
      "Falkland Islands (Malvinas)",
      "Faroe Islands",
      "Fiji",
      "Finland",
      "France",
      "French Guiana",
      "French Polynesia",
      "French Southern Territories",
      "Gabon",
      "Gambia",
      "Georgia",
      "Germany",
      "Ghana",
      "Gibraltar",
      "Greece",
      "Greenland",
      "Grenada",
      "Guadeloupe",
      "Guam",
      "Guatemala",
      "Guernsey",
      "Guinea",
      "Guinea-Bissau",
      "Guyana",
      "Haiti",
      "Heard Island and McDonald Islands",
      "Holy See (Vatican City State)",
      "Honduras",
      "Hong Kong",
      "Hungary",
      "Iceland",
      "India",
      "Indonesia",
      "Iran, Islamic Republic of",
      "Iraq",
      "Ireland",
      "Isle of Man",
      "Israel",
      "Italy",
      "Jamaica",
      "Japan",
      "Jersey",
      "Jordan",
      "Kazakhstan",
      "Kenya",
      "Kiribati",
      "Korea, Democratic People's Republic of",
      "Korea, Republic of",
      "Kuwait",
      "Kyrgyzstan",
      "Lao People's Democratic Republic",
      "Latvia",
      "Lebanon",
      "Lesotho",
      "Liberia",
      "Libyan Arab Jamahiriya",
      "Liechtenstein",
      "Lithuania",
      "Luxembourg",
      "Macao",
      "Macedonia",
      "Madagascar",
      "Malawi",
      "Malaysia",
      "Maldives",
      "Mali",
      "Malta",
      "Marshall Islands",
      "Martinique",
      "Mauritania",
      "Mauritius",
      "Mayotte",
      "Mexico",
      "Micronesia, Federated States of",
      "Moldova, Republic of",
      "Monaco",
      "Mongolia",
      "Montenegro",
      "Montserrat",
      "Morocco",
      "Mozambique",
      "Myanmar",
      "Namibia",
      "Nauru",
      "Nepal",
      "Netherlands",
      "Netherlands Antilles",
      "New Caledonia",
      "New Zealand",
      "Nicaragua",
      "Niger",
      "Nigeria",
      "Niue",
      "Norfolk Island",
      "Northern Mariana Islands",
      "Norway",
      "Oman",
      "Other Country",
      "Pakistan",
      "Palau",
      "Palestinian Territory",
      "Panama",
      "Papua New Guinea",
      "Paraguay",
      "Peru",
      "Philippines",
      "Pitcairn",
      "Poland",
      "Portugal",
      "Puerto Rico",
      "Qatar",
      "Reunion",
      "Romania",
      "Russian Federation",
      "Rwanda",
      "Saint Helena",
      "Saint Kitts and Nevis",
      "Saint Lucia",
      "Saint Pierre and Miquelon",
      "Saint Vincent and the Grenadines",
      "Samoa",
      "San Marino",
      "Sao Tome and Principe",
      "Satellite Provider",
      "Saudi Arabia",
      "Senegal",
      "Serbia",
      "Seychelles",
      "Sierra Leone",
      "Singapore",
      "Slovakia",
      "Slovenia",
      "Solomon Islands",
      "Somalia",
      "South Africa",
      "South Georgia and the South Sandwich Islands",
      "Spain",
      "Sri Lanka",
      "Sudan",
      "Suriname",
      "Svalbard and Jan Mayen",
      "Swaziland",
      "Sweden",
      "Switzerland",
      "Syrian Arab Republic",
      "Taiwan",
      "Tajikistan",
      "Tanzania, United Republic of",
      "Thailand",
      "Timor-Leste",
      "Togo",
      "Tokelau",
      "Tonga",
      "Trinidad and Tobago",
      "Tunisia",
      "Turkey",
      "Turkmenistan",
      "Turks and Caicos Islands",
      "Tuvalu",
      "Uganda",
      "Ukraine",
      "United Arab Emirates",
      "United Kingdom",
      "United States",
      "United States Minor Outlying Islands",
      "Uruguay",
      "Uzbekistan",
      "Vanuatu",
      "Venezuela",
      "Vietnam",
      "Virgin Islands, British",
      "Virgin Islands, U.S.",
      "Wallis and Futuna",
      "Western Sahara",
      "Yemen",
      "Zambia",
      "Zimbabwe",
    );
  }
}
