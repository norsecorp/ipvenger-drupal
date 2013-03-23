
-- SUMMARY --

IPVenger protects your Drupal installation by blocking access by IPs associated
with high-risk behavior.

IPVenger uses NorseCorp IPViking real-time threat assessment technology to
evaluate each IP requesting access to your WordPress site. When a request comes
in, the IP address is sent to the IPViking server, which assigns a numerical
risk factor based on real-time data. IPs whose risk factor exceeds a 
user-configurable threshold are blocked before they can harm your site. Detailed
analytics are provided, as is the ability to blacklist specific IP addresses and
countries of origin. 

For a full description of the module, visit the project page:
  http://www.drupal.org/project/ipvenger.

To submit bug reports and feature suggestions, or to track changes:
  http://drupal.org/project/issues/ipvenger


-- REQUIREMENTS --

* libraries module (>=2.x)

* IPVenger library (available from: tbd)

* Secureimage library (available from: http://www.phpcaptcha.org/download)

(optional -- If also using ipvenger_reports)

* Flot module (https://drupal.org/project/flot) & library
  (http://www.flotcharts.org)

* jVectorMap module (https://drupal.org/project/jvectormap) & library
  (http://jvectormap.com/download/)


-- INSTALLATION --

* Install ipvenger & securimage libraries as usual, for further information see
  http://drupal.org/node/1440066.

* Install this module as usual, for further information see
  http://drupal.org/documentation/install/modules-themes/modules-7.

  (optional -- If also using ipvenger_reports)

* Install Flot module (https://drupal.org/project/flot) & library
  (http://www.flotcharts.org)

* Install jVectorMap module (https://drupal.org/project/jvectormap) & library
  (http://jvectormap.com/download/)


-- CONFIGURATION --

* Go to the IPVenger Settings admin page (admin/config/security/ipvenger).
* Enter your IPVenger API Key.
* Select the type of site that best describes the main function of your website.
* Select who you want block notifications to go to, under 'Email Notification'.


-- FAQ --
Q: How can I test out this module without having a service plan from IPVenger?

A: Add $conf['IPVengerServiceFake'] = TRUE; to your settings.php to give this
   module a test drive.  It will randomly block pages.  Then you can review the
   great analytics available within this module (requires ipvenger_reports).


Q: Sometimes it takes a really long time for the block page to render. Why?

A: This module uses cron to initiate generation of its Captcha images.  If the
   site has turned off cron, consider turning it back on.

-- CONTACT --

Current maintainer:
* Lucas Hedding (heddn) - http://drupal.org/user/1463982
