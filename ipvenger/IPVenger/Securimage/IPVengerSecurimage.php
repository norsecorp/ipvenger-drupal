<?php
/**
 * @file
 * Definition of IPVenger\Securimage\IPVengerSecurimage.
 */

namespace IPVenger\Securimage;

use IPVenger\CMS\IPVengerCacheFactory;
use IPVenger\CMS\IPVengerCacheInterface;
use Securimage;
use Securimage_Color;

class IPVengerSecurimage extends Securimage {

  /**
   * The number of captchas to store in the database.
   */
  const IPVENGER_CAPTCHA_LIMIT = 100;

  protected function output() {
    ob_start();
    imagepng($this->im);
    $image_data = ob_get_clean();

    $cache = IPVengerCacheFactory::get();
    if ($index = $cache->get('ipvenger_captcha_next_index')) {
      $index = $index->data % self::IPVENGER_CAPTCHA_LIMIT;
      $this->captchaCacheSet($index, $image_data);
    }
    else {
      $index = 0;
      $this->captchaCacheSet($index, $image_data);
    }

    imagedestroy($this->im);
    restore_error_handler();
  }

  /**
   * Generate a whole new set of captchas.
   */
  public function initCaptchaCache() {
    $cache = IPVengerCacheFactory::get();
    if ($count = $cache->get('ipvenger_captcha_count')) {
      for ($i = 0; $i < $count->data; $i++) {
        $this->generateCaptcha();
  		}
    }
    else {
      for ($i = 0; $i < self::IPVENGER_CAPTCHA_LIMIT; $i++) {
        $this->generateCaptcha();
      }
      $cache->set('ipvenger_captcha_count', self::IPVENGER_CAPTCHA_LIMIT, IPVengerCacheInterface::IPVENGER_CACHE_PERMANENT);
    }
}

  /**
   * Store the captcha in the database rather than display it.
   */
  private function generateCaptcha() {
    // 1.0 = high distortion, higher numbers = more distortion
    $this->perturbation    = .90;
    // image background color
    $this->image_bg_color  = new Securimage_Color("#CCCCCC");
    // captcha text color
    $this->text_color      = new Securimage_Color("#333333");
    // color of lines over the image
    $this->line_color      = new Securimage_Color("#333333");

    $this->show();
  }

  /**
   * Store the captcha in the database rather than display it.
   */
  private function captchaCacheSet($index, $image_data) {
    $eight_hours = 28800;
    $cache = IPVengerCacheFactory::get();
    $cache->set($index . '_image_data', $image_data, $eight_hours);
    $cache->set($index . '_response', $this->code, $eight_hours);
    $cache->set('ipvenger_captcha_next_index', ++$index, IPVengerCacheInterface::IPVENGER_CACHE_PERMANENT);
  }

  /**
   * Returns the next captcha from the cache for the given IP.
   *
   * When rendering the captchas, we permanently cache the ip address & captcha
   * response pair.  If we are returning a captcha that matches one already
   * rendered to that IP address, then get the next captcha in the list.  If we
   * loop through all 100 captchas for these pairings, then we generate a 100
   * new ones.  Since the person either can't type (and needs a typing break) or
   * they are malicious, they get to wait for the generation to complete.
   *
   * @param string $ipAddress
   *   IP address for which to return a captcha.
   */
  public function getNextCaptcha($ipAddress) {
    $cache = IPVengerCacheFactory::get();
    if ($index = $cache->get('ipvenger_captcha_next_index')) {
      $index = $index->data % self::IPVENGER_CAPTCHA_LIMIT;

      // Have we already generated a captcha for the indexed item?
      // If we haven't then generate one.
      $captchaImageData = $cache->get($index . '_image_data');
      if (!$captchaImageData) {
        $this->generateCaptcha();
        $captchaImageData = $cache->get($index . '_image_data');
      }
      $captchaImageResponse = $cache->get($index . '_response');
      // Generate a new captcha set if we've shown duplicates to a site visitor.
      if ($cache->get($ipAddress . $captchaImageResponse->data)) {
        $this->initCaptchaCache();
        $this->getNextCaptcha($ipAddress);
        return;
      }
      // Remember the ip address / captcha response pairings.
      $cache->set($ipAddress . $captchaImageResponse->data, TRUE, IPVengerCacheInterface::IPVENGER_CACHE_PERMANENT);
    }
    else {
      $index = 0;
      $captchaImageData = $cache->get($index . '_image_data');
      if (!$captchaImageData) {
        $this->generateCaptcha();
        $captchaImageData = $cache->get($index . '_image_data');
      }
      $captchaImageResponse = $cache->get($index . '_response');
    }
    $cache->set('ipvenger_captcha_next_index', ++$index, IPVengerCacheInterface::IPVENGER_CACHE_PERMANENT);

    $_SESSION['ipvenger_captcha_text'] = $captchaImageResponse->data;

    header('Expires: Mon, 26 Jul 1997 05:00:00 GMT');
    header('Last-Modified: ' . gmdate('D, d M Y H:i:s') . 'GMT');
    header('Cache-Control: no-store, no-cache, must-revalidate');
    header('Cache-Control: post-check=0, pre-check=0', false);
    header('Pragma: no-cache');
    header('Content-Type: image/png');

    echo $captchaImageData->data;
  }
}
