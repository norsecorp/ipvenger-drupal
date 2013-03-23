<?php
/**
 * @file
 * Definition of IPVenger\Service\IPVengerService.
 */

namespace IPVenger\Service;

use IPVenger\CMS\IPVengerAppealInterface;
use IPVenger\CMS\IPVengerCacheInterface;
use IPVenger\Service\IPVengerSettings;
use IPVenger\Service\IPVengerServiceData;

/**
 * Defines an abstract class for service interactions with IPVenger.
 *
 * All service implementations have to extend this class.
 *
 */
abstract class IPVengerService {

  /**
   * Checks if an API key isn't valid.
   *
   * It is a negative condition check so you can call
   * <code>
   *   if (!IPVengerService::isApiKeyNotValid('foo')) {
   *     // Things are OK.
   *   }
   *   else {
   *     // Things aren't OK.
   *   }
   * </code>
   *
   * @param string $apiKey
   *   API key to validate.
   *
   * @return string
   *   false | Reason why key is not valid.
   */
  public abstract function isApiKeyNotValid($apiKey);

  /**
   * Returns a risk response for a site vistor.
   *
   * @param IPVengerSettings $settings
   *   The IPVenger settings needed to interact with the IPVenger service.
   * @param IPVengerCacheInterface $cache
   *   A IPVenger cache implementation appropriate for the CMS.
   * @param $ipAddress
   *   The ip address on which to perform a risk assessment.
   *
   * @return IPVengerServiceData
   *   An IPVenger service data.
   */
  public abstract function getRiskResponse(IPVengerSettings $settings, IPVengerCacheInterface $cache, $ipAddress);

  /**
   * Returns a boolean based on the site risk threshold.
   *
   * @param IPVengerServiceData $response
   *   IPVenger service data, passed by reference.
   * @param IPVengerSettings $settings
   *   The IPVenger settings needed to interact with the IPVenger service.
   * @param IPVengerAppealInterface $appeal
   *   A IPVenger appeal implementation appropriate for the CMS.
   *
   * @return boolean
   *   TRUE if the risk is blocked, otherwise FALSE.
   */
  public function assessRiskResponse(IPVengerServiceData &$data, IPVengerSettings $settings, IPVengerAppealInterface $appeal) {
    if ($data->getDisposition() === 0) {
      $block = TRUE;
    }
    elseif ($appeal->validAppealExists($data->getIpAddress())) {
      self::sessionAllow();
      $block = FALSE;
    }
    elseif ($data->getRiskFactor() > $settings->getRiskThreshold()) {
      $data->setDispositionReason('IPQ Score');
      $data->setDisposition(0);
      $block = TRUE;
    }
    elseif ($data->getRiskFactor() <= $settings->getRiskThreshold()) {
      self::sessionAllow();
      $block = FALSE;
    }
    // Default to not block site traffic i.e. FALSE.
    else {
      $block = FALSE;
    }

    // Perform session clean-up.
    if (isset($_SESSION['ipvenger_expire_time'])
          && $_SESSION['ipvenger_expire_time'] >= REQUEST_TIME) {
      unset($_SESSION['ipvenger_status']);
      unset($_SESSION['ipvenger_expire_time']);
    }

    return $block;
  }

  /**
   * When the risk assessment fails, hijack the HTTP flow and render the
   * apprioriate error message.
   *
   * @param IPVengerSettings $settings
   *   The IPVenger settings needed to interact with the IPVenger service.
   * @param IPVengerServiceData $response
   *   IPVenger service data.
   * @param string $basePath
   *   Base path for website.
   */
  public function printErrorMessage(IPVengerSettings $settings, IPVengerServiceData $data, $basePath) {
    // Allow preview of error page.
    $messageType = $data->getCategoryName();
    if ($messageType != 'Botnet') {
      $messageType = isset($_GET['msgType']) ? $_GET['msgType'] : $messageType;
    }

    switch ($messageType) {
      case 'Botnet':
        $message = $settings->getBotnetMessage();
        break;
      case 'Proxy':
        $message = $settings->getProxyMessage();
        break;
      default:
        $message = $settings->getGeneralMessage();
        break;
    }

    if ($settings->getSiteLogo() !== '') {
      $logo = '<img src="' . $settings->getSiteLogo() . '" alt="Logo"/>';
    }
    $siteName = $settings->getSiteName();
    $siteDescription = $settings->getSiteDescription();
    $styleSheet = $settings->getStylesheet();

    $captcha = '';
    if (!in_array($data->getIpAddress(), $settings->getIpAddressBlacklist()) && $settings->getAllowAppeal()  && $messageType != 'Botnet') {
      $captcha = self::captchaForm($data->getIpAddress(), $basePath);
    }

    $html = <<<EOL
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<META http-equiv="Content-Type" content="text/html; charset=utf-8">
<META name="robots" content="noindex" />
<title>IPVenger Block | $siteName</title>
<style type="text/css" media="all">@import url("$styleSheet");
</style>
</head>
<body>
  <header id="branding" role="banner">
  	<hgroup>
  		<h1 id="site-title">$siteName<a></a></h1>
  		<h2 id="site-description">$siteDescription</h2>
  	</hgroup>
  	$logo
  </header>

  <div id="main" class="wrap ipvenger-block">
  <div id="ipvenger-user-block-message">
$message
  </div>
$captcha
	<div class="protected-by">
		<h3>Protected by IPVenger</h3>
	</div>
  </div>
</body>
</html>
EOL;

    print $html;
    die;
  }

  /**
   * Block page submit handler.
   *
   * @param IPVengerServiceData $response
   *   IPVenger service data.
   * @param IPVengerAppealInterface $appeal
   *   A IPVenger appeal implementation appropriate for the CMS.
   * @param string $appealEmailAddress
   *   The email address to send appeals.
   * @param string $controlCenterURL
   *   Full URL for the site to the IPVenger control center.
   */
  public function processFormSubmit(IPVengerServiceData $data, IPVengerAppealInterface $appeal, $appealEmailAddress, $controlCenterURL) {
    $validate = FALSE;

    $visitorIpAddress = $_POST['ip'];
    $visitorEmail = $_POST['email'];
    if ($data->getIpAddress() != $visitorIpAddress) {
      // Attempting to appeal an IP address that doesn't
      // match what we think it should be?
      self::unexpectedError();
    }
    elseif (!self::validateEmail($visitorEmail)) {
      $_SESSION['ipvenger_form_error'] = 'A valid email address and CAPTCHA response are required to submit an appeal.';
    }
    elseif (!self::validateCaptcha()) {
      $_SESSION['ipvenger_form_error'] = 'A valid email address and CAPTCHA response are required to submit an appeal.';
    }
    else {
      $appeal->set($visitorIpAddress, $visitorEmail);
      $message = self::emailAppealMessage($data, $visitorEmail, $controlCenterURL);
      mail($appealEmailAddress, 'IPVenger Appeal Notification', $message, 'Content-type: text/html');

      $validate = TRUE;
    }

    return $validate;
  }

  /**
   * Helper function to print a generic error message.
   *
   * This likely occurs due to CSRF or some other nasty hackery so no need to
   * make the message too pretty or revealing.
   */
  public function unexpectedError() {
    $html = <<<EOL
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<META http-equiv="Content-Type" content="text/html; charset=utf-8">
<title>IPVenger Block</title>
</style>
</head>
<body>
	<div>
		<p>An unexpected error occured.  Please try again.</p>
	</div>
</div>
</body>
</html>
EOL;

    print $html;
    die;
  }

  /**
   *  Helper function to return a formatted emal message.
   *
   * @param IPVengerServiceData $response
   *   IPVenger service data.
   * @param string $visitorEmail
   *   Email address of the site visitor.
   * @param string $controlCenterURL
   *   Full URL for the site to the IPVenger control center.
   *
   * @return string
   *   Formatted email message to send site administrator.
   */
  private function emailAppealMessage(IPVengerServiceData $data, $visitorEmail, $controlCenterURL){
    $ipAddress = $data->getIpAddress();
    $reason = $data->getDispositionReason();
    $category = '';
    if ($reason == 'IPQ Score' && $data->getFactorName() == 'IPViking Category Factor') {
      $category = 'Category: ' . $data->getCategoryName() . PHP_EOL;
    }

    $message = <<<EOM
<html><body>
You are receiving this email because a visitor was blocked from accessing
your website by IPVenger and has appealed this action. The visitor supplied
a valid email address and CAPTCHA challenge response and has been granted
temporary access to your site.<p>

<strong>Details:</strong>

<pre style="font-size:120%">
IP Address:	$ipAddress
Block Reason:	$reason
${category}Visitor Email:	<a href="mailto:$visitorEmail">$visitorEmail</a>
</pre>

This user's access will automatically expire within 48 hours and they
may appeal again at that time.<p>

If you choose to take action, you can whitelist this visitor's IP address,
or blacklist it and prevent further appeals from the
<a href="$controlCenterURL">IPVenger IP Control Panel.</a>
<p>

</body></html>
EOM;

    return $message;
  }

  /**
   * Helper function to return the captcha html.
   *
   * @param string $ipAdddress
   *   IP address for which to generate a captcha.
   * @param string $basePath
   *   Base path for captcha image.
   *
   * @return string
   */
  private function captchaForm($ipAdddress, $basePath) {
    $errorMsg = isset($_SESSION['ipvenger_form_error']) ? $_SESSION['ipvenger_form_error'] : '';
    unset($_SESSION['ipvenger_form_error']);
    $captchaPath = $basePath . 'ipvenger/pages/captcha';
    $captcha = <<<EOC
	<div id="ipvenger-block-appeal-form">

	<p id="ipvenger-block-appeal-msg1">
		To appeal this block and gain temporary access to the site,
		please fill in the information below.
	</p>
	<p id="ipvenger-block-appeal-msg2">
	The website administrator will be notified of your appeal.
	</p>

	<form method=POST>
		<input type="hidden" name="ip" value=$ipAdddress />
		<div id="ipvenger-form-error">$errorMsg</div>
		<div id="ipvenger-block-email-container">
			<span id="ipvenger-block-email-prompt">Your email address: </span>
			<input type="text" name="email" id="ipvenger-appeal-email" /><br>
		</div>

		<div id="ipvenger-block-captcha-container">

			<div id="ipvenger-block-captcha-image-container">
				<img id="ipvenger-block-captcha"
					src = "$captchaPath?ip=$ipAdddress" />
			</div>

			<div id="ipvenger-captcha-controls">

				<span id="ipvenger-captcha-prompt"> Text shown in image:  </span>
				<input type="text" name="captcha_response"
					id="ipvenger-captcha-response" />
				<br>
				<a href="javascript:window.location.href=window.location.href">[ Try a Different Image ]</a>

			</div>

		</div>

		<div id="ipvenger-submit-container">
			<button type=submit id="ipvenger-submit-appeal">Appeal</button>
		</div>
	</div>
EOC;
    return $captcha;
  }

  /**
   * Helper function to remember the site visitor so we don't make as many calls
   * to the API.
   */
  private function sessionAllow() {
    $_SESSION['ipvenger_status'] = 'allow';
    // Expire session after 48 hours to force api call (48h = 172800s).
    $_SESSION['ipvenger_expire_time'] = REQUEST_TIME + 172800;
  }

  /**
   * Helper function to validate an email address.
   */
  private function validateEmail($email) {
    do {
      if (empty($email)) {
        break;
      }

      $atIndex = strrpos($email, "@");

      if (is_bool($atIndex) && !$atIndex) {
        break;
      }
      else {
        $domain = substr($email, $atIndex + 1);
        $local = substr($email, 0, $atIndex);
      }

      $localLen = strlen($local);
      $domainLen = strlen($domain);

      if ($localLen < 1 || $localLen > 64) {
        break;
      }
      else if ($domainLen < 1 || $domainLen > 255) {
        break;
      }

      if (!preg_match('/^(\\\\.|[A-Za-z0-9!#%&`_=\\/$\'*+?^{}|~.-])+$/', str_replace("\\\\", "", $local))) {
        if (!preg_match('/^"(\\\\"|[^"])+"$/', str_replace("\\\\", "", $local))) {
          break;
        }
      }

      if (!preg_match('/^[A-Za-z0-9\\-\\.]+$/', $domain)) {
        break;
      }
      else if (preg_match('/\\.\\./', $domain)) {
        break;
      }
      else if (function_exists("checkdnsrr") && !(checkdnsrr($domain, "MX") || checkdnsrr($domain, "A"))) {
        break;
      }

      return TRUE;

    }
    while (FALSE);

    return FALSE;
  }

  /**
   * Helper function to validate the captcha.
   *
   * @param
   *   captcha string
   */
  private function validateCaptcha() {
    $return =
    $_SESSION['ipvenger_captcha_text'] == strtolower($_POST['captcha_response']);
    return $return;
  }

}
