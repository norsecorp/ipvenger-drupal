<?php
/**
 * @file
 * Administrative page callbacks for the IP Venger module.
 */

use IPVenger\Service\IPVengerSettings;
use IPVenger\Service\IPVengerServiceFactory;

/**
 * Generate a form for configuring IP Venger standard settings.
 */
function ipvenger_admin_settings($form, &$form_state) {
  $form['ipvenger_apikey'] = array(
    '#title' => t('IPVenger API Settings'),
    '#type' => 'textfield',
    '#default_value' => variable_get('ipvenger_apikey', ''),
    '#size' => 100,
    '#required' => TRUE,
    '#element_validate' => array('ipvenger_apikey_validate'),
    '#description' => t('Click "Save Configuration" at the bottom of the page for any settings changes to take affect'),

  );

  $form['ipvenger-security-settings'] = array(
    '#type' => 'item',
    '#markup' => '<div class="frame-header">SECURITY SETTINGS</div>
    <p id="ipv-site-type-header">The main function of my website is (choose one):
      <span class="ivenger-tooltip" title="' . t('Choose the one option that best describes your website. The site type determines the maximum IPQ score that should be deemed an acceptable risk. The IPQ threshold assigned to each site type is based on many factors, among them how attractive sites of this type are to attackers, and how likely an attack is to succeed if access to the site is allowed. These factors are weighed against the relative cost and inconvenience of inadvertently blocking a legitimate user.') . '"> [?]</span>
      </p>',
  );

  // If we have a value for ipvenger_site_function from $form_state['values']
  // we use this both as the default value for ipvenger_site_function and also
  // as a parameter to pass to the function that retrieves the value for the
  // ipvenger_risk_threshold.
  if (isset($form_state['values']['ipvenger_site_function'])) {
    $site_function = $form_state['values']['ipvenger_site_function'];
  }
  else {
    $site_function = variable_get('ipvenger_site_function', 'default');
  }

  $options = array(
    'default' => t('Default - recommended security level appropriate for most typical websites'),
    'marketing' => t('Marketing - consumer education and marketing with no sensitive corporate or consumer data'),
    'social_platform' => t('Social Platform - a social networking site or web forum with multiple users'),
    'corporate' => t('Corporate - a high traffic, high visibility site that supports a widely recognized brand'),
    'blog' => t('Blog - a personal blog that allows user comments'),
    'web_app' => t('Web App - a software-as-a-service (SaaS) product that stores account information'),
    'ecommerce' => t('eCommerce - processing payments, purchases, or transactions containing sensitive data'),
    'custom' => t('Custom - specify the IPVenger Risk Threshold manually.'),
  );

  $form['ipvenger_site_function'] = array(
    '#type' => 'radios',
    '#default_value' => $site_function,
    '#options' => $options,
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => 'ipvenger_risk_threshold_callback',
      'wrapper' => 'ipvenger_risk_threshold',
    ),
  );
  $form['ipvenger_risk_threshold'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('ipvenger_risk_threshold', _ipvenger_risk_threshold_value($site_function)),
    '#description' => _ipvenger_risk_threshold_description($site_function),
    '#size' => 2,
    '#maxlength' => 2,
    '#required' => TRUE,
    '#disabled' => $site_function == 'custom' ? FALSE : TRUE,
    // The prefix/suffix provide the div that we're replacing, named by
    // #ajax['wrapper'] in 'ipvenger_site_function'.
    '#prefix' => '<div id="ipvenger-risk-threshold">',
    '#suffix' => '</div>',
    '#element_validate' => array('element_validate_integer_positive'),
  );

  // An AJAX request calls the form builder function for every change.
  // We can change how we build the form based on $form_state.
  if (!empty($form_state['values']['ipvenger_site_function'])) {
    $form['ipvenger_risk_threshold']['#default_value'] = _ipvenger_risk_threshold_value($site_function);
  }

  $form['ipvenger_appeal_allow'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow blocked users to appeal.')
    . '<span class="ivenger-tooltip" title="' . t('By default, a blocked user can gain temporary access to your site by providing a valid e-mail address and responding to a CAPTCHA. This allows legitimate human users to access your site even though they may have a high IPQ score, while still stopping most automated attackers (Bots). Unchecking this box will turn off the ability for a user to appeal. This will increase security but may also block some legitimate users from accessing the site.') . '"> [?]</span>',
    '#default_value' => variable_get('ipvenger_appeal_allow', TRUE),
    '#description' => t('When a visitor to your site is blocked, they can appeal and gain access to your site for 48 hours by providing their email address. You can disable the ability for users to appeal by unchecking the box above.'),
    '#ajax' => array(
      'callback' => 'ipvenger_apeal_email_callback',
      'wrapper' => 'ipvenger-appeal-email',
    ),
    '#prefix' => '<label>Allow Appeal</label>',
  );

  $form['ipvenger_appeal_email'] = array(
    // The prefix/suffix provide the div that we're replacing, named by
    // #ajax['wrapper'] above.
    '#prefix' => '<div id="ipvenger-appeal-email">',
    '#suffix' => '</div>',
  );
  // An AJAX request calls the form builder function for every change.
  // We can change how we build the form based on $form_state.
  if (isset($form_state['values']['ipvenger_appeal_allow'])) {
    $appeal_allow = $form_state['values']['ipvenger_appeal_allow'];
  }
  else {
    $appeal_allow = variable_get('ipvenger_appeal_allow', TRUE);
  }
  if ($appeal_allow) {
    $account = user_load(0);
    $form['ipvenger_appeal_email']['#type'] = 'textfield';
    $form['ipvenger_appeal_email']['#title'] = t('Email Notification');
    $form['ipvenger_appeal_email']['#default_value'] = variable_get('ipvenger_appeal_email', $account->mail);
    $form['ipvenger_appeal_email']['#description'] = t('An email will be sent to the address you choose, giving the opportunity to whitelist or blacklist the IP in question.');
    $form['ipvenger_appeal_email']['#required'] = TRUE;
    $form['ipvenger_appeal_email']['#element_validate'] = array('ipvenger_appeal_email_validate');
  }

  $form['ipvenger_general_msg'] = array(
    '#title' => t('General Message')
    . '<span class="ivenger-tooltip" title="' . t('When IPVenger blocks a user from accessing your site, they are shown an error page. You can customize what the blocked user sees by modifying this HTML. This is the message that most blocked users will see.') . '"> [?]</span>',
    '#type' => 'textarea',
    '#description' => t('The message that you want to display to blocked IP traffic on the IPVenger landing page. Preview the page !url', array(
      '!url' => l(t('here'), 'ipvenger/pages/block',
        array(
          'attributes' => array('target' => '_blank'),
        )
      ),
    )),
    '#default_value' => variable_get('ipvenger_general_msg', '<h1>Forbidden</h1><p>Your access to this website has been blocked because your IP address has been identified as a potential threat.  If you believe that this is an error, and wish to be allowed access, please contact the site administrator.</p>'),
  );

  $form['ipvenger_proxy_msg'] = array(
    '#title' => t('Proxy Message')
    . '<span class="ivenger-tooltip" title="' . t('A proxy server acts as an intermediary between a user and the Internet. Proxies are often used by schools and companies to block access to undesirable sites. However, they can also be used by attackers to hide their identity. This message is shown to blocked users who are behind a proxy. You can customize what the blocked user sees by modifying this HTML.') . '"> [?]</span>',
    '#type' => 'textarea',
    '#description' => t('The message that you want to display to a blocked IP that is using a proxy. Preview the page !url', array(
      '!url' => l(t('here'), 'ipvenger/pages/block',
        array(
          'query' => array('msgType' => 'Proxy'),
          'attributes' => array('target' => '_blank'),
        )
      ),
    )),
    '#default_value' => variable_get('ipvenger_prox_msg', '<h1>Forbidden</h1><p>Your access to this website has been blocked because you are accessing the Internet through a Proxy that has been identified as a potential threat.  If you believe that this is an error, and wish to be allowed access, please contact the site administrator.</p>'),
  );

  $form['ipvenger_botnet_msg'] = array(
    '#title' => t('Botnet Message')
    . '<span class="ivenger-tooltip" title="' . t('A Botnet is a collection of computers infected with malicious software that may be under the control of an attacker. In most cases, the legitimate owner of the computer is unaware that their computer is part of a Botnet. Requests from computers that are part of a Botnet are extremely dangerous and will generally be blocked by IPVenger. This message is shown to blocked users who are part of a Botnet. You can customize what the blocked user sees by modifying this HTML.') . '"> [?]</span>',
    '#type' => 'textarea',
    '#description' => t('The message that you want to display to a blocked IP that is a member of a Botnet. Preview the page !url', array(
      '!url' => l(t('here'), 'ipvenger/pages/block',
        array(
          'query' => array('msgType' => 'Botnet'),
          'attributes' => array('target' => '_blank'),
        )
      ),
    )),
    '#default_value' => variable_get('ipvenger_botnet_msg', '<h1>Forbidden</h1><p>Your access to this website has been blocked because the computer you are using appears to have been hijacked by malicious software.  Please update your antivirus and antimalware software and contact the site administrator to request access if the problem persists.</p>'),
  );

  $form['#attached']['css'] = array(
    drupal_get_path('module', 'ipvenger') . '/css/ipvenger.css' => array(
      'type' => 'file',
    ),
  );

  return system_settings_form($form);
}

/**
 * Elemement validate function for 'ipvenger_apikey'.
 */
function ipvenger_apikey_validate($element, &$form_state, $form) {
  $service = IPVengerServiceFactory::get();
  if ($reason = $service->isApiKeyNotValid($element['#value'])) {
    form_error($element, t('The API key is not valid because: @reason', array(
      '@reason' => $reason,
    )));
  }
  else {
    variable_set('ipvenger_apikey_valid', TRUE);

    // Generate captchas using drupal's queue solution to decrease wait time
    // at validate.
    $queue = DrupalQueue::get('ipvenger', TRUE);
    $queue->createQueue();
    $queue->createItem('generateCaptchas');
  }
}

/**
 * Elemement validate function for 'ipvenger_appeal_email'.
 */
function ipvenger_appeal_email_validate($element, &$form_state, $form) {
  if (!valid_email_address($element['#value'])) {
    form_error($element, t('Please enter a valid email address.'));
  }
}

/**
 * Callback element need only select the portion of the form to be updated.
 *
 * @return array
 *   renderable array
 */
function ipvenger_risk_threshold_callback($form, &$form_state) {
  return $form['ipvenger_risk_threshold'];
}

/**
 * Callback element need only select the portion of the form to be updated.
 *
 * @return array
 *   renderable array
 */
function ipvenger_apeal_email_callback($form, &$form_state) {
  $form_state['rebuild'] = TRUE;

  return $form['ipvenger_appeal_email'];
}

/**
 * Generate a form for configuring IP Venger advanced settings.
 */
function ipvenger_admin_advanced_settings($form, &$form_state) {

  $form['ipvenger-ip-white-black'] = array(
    '#type' => 'fieldset',
    '#title' => t('IP Address Settings'),
    '#description' => t('Seperate each IP address with a new line.  An asterisk may be used as a wildcard character, for example 10.1.1.* would block the IPs 10.1.1.1, 10.1.1.2, etc.'),
  );

  $form['ipvenger-ip-white-black']['ipvenger-ip-white-black-warning'] = array(
    '#type' => 'item',
    '#title' => t('Warning'),
    '#markup' => '<div class="ipvenger-warning">' . t('Blacklisting an IP will block all access attempts from that address, regardless of IPQ score. IP Blacklists should be used with care. Because some IP addresses are dynamic (the user behind the IP may change), blacklisting a single IP may not protect from future attacks. In fact, an IP used by an attacker one day may belong to a legitimate user on another.') . '</div>',
  );

  $result = db_query('SELECT mask FROM {ipvenger_exception} WHERE exceptionType = :type AND action = :action',
    array(':type' => 'ip', ':action' => 'allow'))
    ->fetchCol();
  $form['ipvenger-ip-white-black']['ipvenger-ip-whitelist'] = array(
    '#type' => 'textarea',
    '#title' => t('Whitelist'),
    '#description' => t('Whitelisting an IP will allow all access attempts from that address, regardless of IPQ score.'),
    '#default_value' => implode(PHP_EOL, $result),
    '#element_validate' => array('ipvenger_ip_address_validate'),
  );

  $result = db_query('SELECT mask FROM {ipvenger_exception} WHERE exceptionType = :type AND action = :action',
    array(':type' => 'ip', ':action' => 'deny'))
    ->fetchCol();
  $form['ipvenger-ip-white-black']['ipvenger-ip-blacklist'] = array(
    '#type' => 'textarea',
    '#title' => t('Blacklist'),
    '#description' => t('Blacklisting an IP will dis-allow all access attempts from that address, regardless of IPQ score.'),
    '#default_value' => implode(PHP_EOL, $result),
    '#element_validate' => array('ipvenger_ip_address_validate'),
  );

  // We only need to load the country list if the ipvenger library is available.
  $library = libraries_load('ipvenger');
  if ($library['installed']) {
    $result = db_query('SELECT mask FROM {ipvenger_exception} WHERE exceptionType = :type AND action = :action',
      array(':type' => 'country', ':action' => 'deny'))
      ->fetchCol();
    $form['ipvenger-country-blacklist-wrapper'] = array(
      '#type' => 'container',
    );

    $form['ipvenger-country-blacklist-wrapper']['country-warning'] = array(
      '#type' => 'item',
      '#title' => t('Warning'),
      '#markup' => '<div class="ipvenger-warning">' . t('Blacklisting an entire country will block all access attempts from that country including any legitimate users. Country blacklists should be used with extreme caution. Related attacks are often seen to come from geographic areas that span political borders. Because of this, blacklisting one country may have the effect of raising the IPQ score of requests from neighboring countries.') . '</div>',
    );
    $form['ipvenger-country-blacklist-wrapper']['country-list'] = array(
      '#type' => 'select',
      '#title' => t('Blacklist Countries'),
      '#options' => drupal_map_assoc(IPVengerSettings::getCountryList()),
      '#multiple' => TRUE,
      '#description' => t('Warning: Blacklisting a country may affect the behavior of IPVenger in ways you do not anticipate.'),
      '#default_value' => drupal_map_assoc($result),
      '#size' => 12,
    );

    if (libraries_detect('jvectormap')) {
      $form['ipvenger-country-blacklist-wrapper']['country-map'] = array(
        '#theme' => 'jvectormap',
        '#attributes' => array(
          'id' => 'ipvenger-country-map',
          'style' => 'width: 400px; height: 240px'),
      );

      $form['#attached']['js'] = array(
        drupal_get_path('module', 'ipvenger') . '/js/ipvenger.js' => array(
          'type' => 'file',
        ),
        libraries_get_path('ipvenger', TRUE) . '/js/ipvenger_country_codes.js' => array(
          'type' => 'file',
        ),
        libraries_get_path('ipvenger', TRUE) . '/js/ipvenger_world_en.js' => array(
          'type' => 'file',
        ),
      );

      // Blacklisted country names.
      $query = db_query('SELECT mask FROM {ipvenger_exception} WHERE exceptionType = :type AND action = :action',
          array(':type' => 'country', ':action' => 'deny'));
      $results = $query->fetchCol();
      $settings['blocked_countries'] = array();
      foreach ($results as $result) {
        $settings['blocked_countries'][] = $result;
      }

      // Blocked country label details.
      $query = db_select('ipvenger_request_detail', 'd');
      $query->addField('d', 'country', 'label');
      $query->addExpression('AVG(riskFactor)', 'average_score');
      $query->addExpression('SUM(NOT disposition)/COUNT(*) * 100', 'percentage');
      $query->groupBy('label');
      $results = $query->execute()->fetchAll();
      foreach ($results as $result) {
        $settings['country_labels'][] = $result;
      }
      $form['#attached']['js'][] = array(
        'data' => array('ipvenger' => $settings),
        'type' => 'setting',
      );
    }
  }

  $options = array(
    604800 => t('Last 7 days'),
    1209600 => t('Last 14 days'),
    2592000 => t('Last 30 days'),
  );
  $form['ipvenger-data-export'] = array(
    '#type' => 'select',
    '#title' => t('Export Reporting Data')
    . '<span class="ivenger-tooltip" title="' . t('Detailed traffic records for the last 30 days can be downloaded by administrators for archival or analysis at any time.') . '"> [?]</span>',
    '#options' => $options,
    '#default_value' => 2592000,
    '#description' => t('Exports data in CSV file format.'),
  );
  $form['ipvenger-data-export-submit'] = array(
    '#type' => 'submit',
    '#value' => t('Download'),
    '#submit' => array('_ipvenger_data_export_submit'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Elemement validate function for 'ipvenger_ip_whitelist'.
 */
function ipvenger_ip_address_validate($element, &$form_state, $form) {
  $ip_addresses = preg_split('/\n|\r/', $element['#value'], -1, PREG_SPLIT_NO_EMPTY);
  foreach ($ip_addresses as $ip_address) {
    if (!_ipvenger_ip_address_validator($ip_address)) {
      form_error($element,
        t('IP address: @ip_address must use whole numbers (0-254) or asterisk (*).',
          array('@ip_address' => $ip_address)));
    }
  }
}

/**
 * Validate handler for ipvenger_admin_advanced_settings().
 */
function ipvenger_admin_advanced_settings_validate($form, &$form_state) {
  // Make sure that there are no duplicate IP addresses
  // before we throw a PDOException.
  $whitelist_ip_addresses = preg_split('/\n|\r/', $form_state['values']['ipvenger-ip-whitelist'], -1, PREG_SPLIT_NO_EMPTY);
  $blacklist_ip_addresses = preg_split('/\n|\r/', $form_state['values']['ipvenger-ip-blacklist'], -1, PREG_SPLIT_NO_EMPTY);

  if ($ip_addresses = array_intersect($whitelist_ip_addresses, $blacklist_ip_addresses)) {
    form_set_error('ipvenger_ip_blacklist',
      t('Duplicate IP address(es): @ip_addresses.',
        array('@ip_addresses' => implode(', ', $ip_addresses))));
  }
}

/**
 * Submit handler for ipvenger_admin_advanced_settings().
 *
 * Inserts and deletes the settings data into {ipvenger_exception}.
 */
function ipvenger_admin_advanced_settings_submit($form, &$form_state) {
  $whitelist_ip_addresses = preg_split('/\n|\r/', $form_state['values']['ipvenger-ip-whitelist'], -1, PREG_SPLIT_NO_EMPTY);
  $blacklist_ip_addresses = preg_split('/\n|\r/', $form_state['values']['ipvenger-ip-blacklist'], -1, PREG_SPLIT_NO_EMPTY);
  $blacklist_countries = array_keys($form_state['values']['country-list']);

  // Clean-up values in database for ip whitelist.
  $result = db_query('SELECT mask FROM {ipvenger_exception} WHERE exceptionType = :type AND action = :action',
    array(
      ':type' => 'ip',
      ':action' => 'allow',
    ))
    ->fetchCol();
  $delete_rows = array_diff($result, $whitelist_ip_addresses);
  foreach ($delete_rows as $delete_row) {
    db_delete('ipvenger_exception')
      ->condition('exceptionType', 'ip')
      ->condition('action', 'allow')
      ->condition('mask', $delete_row)
      ->execute();
  }

  // Clean-up values in database for ip blacklist.
  $result = db_query('SELECT mask FROM {ipvenger_exception} WHERE exceptionType = :type AND action = :action',
    array(
      ':type' => 'ip',
      ':action' => 'deny',
    ))
    ->fetchCol();
  $delete_rows = array_diff($result, $blacklist_ip_addresses);
  foreach ($delete_rows as $delete_row) {
    db_delete('ipvenger_exception')
      ->condition('exceptionType', 'ip')
      ->condition('action', 'deny')
      ->condition('mask', $delete_row)
      ->execute();
  }

  // Clean-up values in database for country blacklist.
  $result = db_query('SELECT mask FROM {ipvenger_exception} WHERE exceptionType = :type AND action = :action',
    array(
      ':type' => 'country',
      ':action' => 'deny',
    ))
    ->fetchCol();
  $delete_rows = array_diff($result, $blacklist_countries);
  foreach ($delete_rows as $delete_row) {
    db_delete('ipvenger_exception')
      ->condition('exceptionType', 'country')
      ->condition('action', 'deny')
      ->condition('mask', $delete_row)
      ->execute();
  }

  // Add new whitelist ip entries.
  foreach ($whitelist_ip_addresses as $whitelist_ip_address) {
    _ipvenger_merge_ip_address_exception('allow', $whitelist_ip_address);
  }

  // Add new blacklist ip entries.
  foreach ($blacklist_ip_addresses as $blacklist_ip_address) {
    _ipvenger_merge_ip_address_exception('deny', $blacklist_ip_address);
  }

  // Add new blacklist country entries.
  foreach ($blacklist_countries as $blacklist_country) {
    db_merge('ipvenger_exception')
      ->key(array(
          'mask' => $blacklist_country,
          'exceptionType' => 'country',
          'action' => 'deny',
        ))
      ->fields(array(
          'mask' => $blacklist_country,
          'exceptionType' => 'country',
          'action' => 'deny',
        ))
      ->execute();
  }

  drupal_set_message(t('The configuration options have been saved.'));
}

/**
 * Generate a form for administering and blocking IP addresses.
 */
function ipvenger_admin_control_center($form, &$form_state) {

  $form['ip-address-wrapper'] = array(
    '#type' => 'fieldset',
  );
  $form['ip-address-wrapper']['ip-address-lookup-title'] = array(
    '#markup' => '<h2>' . t('IP Lookup') . '</h2>',
  );
  $form['ip-address-wrapper']['ip-address-lookup-description-header'] = array(
    '#type' => 'item',
    '#title' => t('Look up a specific IP address to see its history on your site.')
    . '<span class="ivenger-tooltip" title="' . t('Enter a specific IP address to see details about all requests from that IP in the last 30 days. The history includes both blocked and allowed requests. Note that data is only available for IP Addresses that have actually attempted to access your website.') . '"> [?]</span>',
  );
  $form['ip-address-wrapper']['ip-address-lookup'] = array(
    '#type' => 'container',
  );
  $form['ip-address-wrapper']['ip-address-lookup']['left'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('ipvenger-left'),
    ),
  );
  $form['ip-address-wrapper']['ip-address-lookup']['right'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('ipvenger-right'),
    ),
  );
  $form['ip-address-wrapper']['ip-address-lookup']['left']['ip-lookup-textfield'] = array(
    '#type' => 'textfield',
    '#size' => 15,
    '#maxlength' => 15,
    '#attributes' => array(
      'class' => array('ipvenger-ip-address'),
    ),
  );
  $form['ip-address-wrapper']['ip-address-lookup']['right']['ip-lookup-submit'] = array(
    '#type' => 'submit',
    '#value' => t('Lookup'),
    '#submit' => array('ipvenger_admin_control_center_ip_lookup'),
    '#disabled' => TRUE,
    '#executes_submit_callback' => FALSE,
    '#attributes' => array(
      'class' => array('ipvenger-ip-address'),
      'onclick' => 'return (false);',
    ),
  );
  $form['ip-address-wrapper']['ip-address-lookup-description-footer'] = array(
    '#markup' => '<div id="ip-address-lookup-description-footer" class="description ipvenger-clear">' . t('Enter a specific IP address to see details about all requests from that IP in the last 30 days. The history includes both blocked and allowed requests. Note that data is only available for IP Addresses that have actually attempted to access your website.') . '</div>',
  );

  $query = db_select('ipvenger_request_detail', 'd')
    ->extend('PagerDefault')
    ->orderBy('timestamp', 'DESC')
    ->limit(20);
  $query->leftJoin('ipvenger_appeal', 'a', '(a.ipAddress = d.ipAddress)');
  $query
    ->fields('d',
      array(
        'ipAddress',
        'timestamp',
        'country',
        'organization',
        'riskFactor',
        'dispositionReason',
        'factorName',
        'categoryName',
      ));
  $query->fields('a', array('appealId'));
  $query->condition('disposition', 0);

  // An AJAX request calls the form builder function for every change.
  // We can change how we build the form based on $form_state.
  if (isset($form_state['values']['start-date'])) {
    $date_parts = explode('-', $form_state['values']['start-date']);
    $year = isset($date_parts[0]) ? $date_parts[0] : NULL;
    $month = isset($date_parts[0]) ? $date_parts[1] : NULL;
    $day = isset($date_parts[0]) ? $date_parts[2] : NULL;
    if (checkdate($month, $day, $year)) {
      $query->condition('d.timestamp', mktime(0, 0, 0, $month, $day, $year), '>=');
    }
    else {
      $query->condition('d.timestamp', mktime(0, 0, 0), '>=');
    }
  }
  if (isset($form_state['values']['end-date'])) {
    $date_parts = explode('-', $form_state['values']['end-date']);
    $year = isset($date_parts[0]) ? $date_parts[0] : NULL;
    $month = isset($date_parts[0]) ? $date_parts[1] : NULL;
    $day = isset($date_parts[0]) ? $date_parts[2] : NULL;
    if (checkdate($month, $day, $year)) {
      $query->condition('d.timestamp', mktime(23, 59, 59, $month, $day, $year), '<=');
    }
    else {
      $query->condition('d.timestamp', mktime(23, 59, 59), '<=');
    }
  }
  if (isset($form_state['values']['country'])) {
    $country = $form_state['values']['country'];
    if ($country != 'all') {
      $query->condition('country', $country);
    }
  }
  if (isset($form_state['values']['disposition'])) {
    $disposition = $form_state['values']['disposition'];
    if ($disposition != 'all') {
      $query->condition('dispositionReason', $disposition);
    }
  }
  if (isset($form_state['values']['category-filter'])) {
    $category = $form_state['values']['category-filter'];
    if ($category != 'all') {
      $query->condition(
        db_or()
          ->condition('factorName', $category)
          ->condition(
            db_and()
              ->condition('factorName', 'IPViking Category Factor')
              ->condition('categoryName', $category)
          )
      );
    }
  }
  if (isset($form_state['values']['appeal'])) {
    if ($form_state['values']['appeal']) {
      $query->isNotNull('a.appealId');
    }
  }
  $result = $query
    ->execute()
    ->fetchAll(PDO::FETCH_ASSOC);

  $rows = array_map('_ipvenger_admin_control_center_row_callback', $result);

  $form['table-filter-wrapper'] = array(
    '#type' => 'fieldset',
    '#attributes' => array(
      'class' => array('ipvenger'),
    ),
  );
  $form['table-filter-wrapper']['table-title'] = array(
    '#markup' => '<h2>' . t('IPs Blocked') . '</h2>',
  );
  $form['table-filter-wrapper']['table-filter'] = array(
    '#type' => 'container',
  );
  $form['table-filter-wrapper']['table-filter']['description'] = array(
    '#markup' => '<div id="ipvenger-table-filter-by" class="ipvenger-table-filter">' . t('Filter results by:') . '</div>',
  );
  $form['table-filter-wrapper']['table-filter']['start-date'] = array(
    '#type' => 'textfield',
    '#title' => t('Start Date'),
    '#default_value' => date('Y-m-d'),
    '#size' => 10,
    '#maxlength' => 10,
    '#ajax' => array(
      'callback' => 'ipvenger_control_center_table_callback',
      'wrapper' => 'ipvenger-table',
    ),
    '#attributes' => array('class' => array('ipvenger-table-filter')),
  );
  $form['table-filter-wrapper']['table-filter']['end-date'] = array(
    '#type' => 'textfield',
    '#title' => t('End Date'),
    '#default_value' => date('Y-m-d'),
    '#size' => 10,
    '#maxlength' => 10,
    '#ajax' => array(
      'callback' => 'ipvenger_control_center_table_callback',
      'wrapper' => 'ipvenger-table',
    ),
    '#attributes' => array('class' => array('ipvenger-table-filter')),
  );
  // We only need to load the country list if the ipvenger library is available.
  $library = libraries_load('ipvenger');
  if ($library['installed']) {
    $form['table-filter-wrapper']['table-filter']['country'] = array(
      '#type' => 'select',
      '#title' => t('Country'),
      '#options' => drupal_map_assoc(IPVengerSettings::getCountryList()),
      '#empty_option' => t('All'),
      '#empty_value' => 'all',
      '#ajax' => array(
        'callback' => 'ipvenger_control_center_table_callback',
        'wrapper' => 'ipvenger-table',
      ),
      '#attributes' => array('class' => array('ipvenger-table-filter')),
    );
  }
  $result = db_query('SELECT DISTINCT dispositionReason FROM {ipvenger_request_detail}')->fetchCol();
  $form['table-filter-wrapper']['table-filter']['disposition'] = array(
    '#type' => 'select',
    '#title' => t('Disposition'),
    '#options' => drupal_map_assoc(array_values($result)),
    '#empty_option' => t('All'),
    '#empty_value' => 'all',
    '#ajax' => array(
      'callback' => 'ipvenger_control_center_table_callback',
      'wrapper' => 'ipvenger-table',
    ),
    '#attributes' => array('class' => array('ipvenger-table-filter')),
  );
  $category_names = db_select('ipvenger_request_detail', 'd')
    ->fields('d', array('categoryName'))
    ->condition('categoryName', 'None', '<>')
    ->condition('disposition', 0)
    ->distinct()
    ->execute()
    ->fetchCol();
  $factor_names = db_select('ipvenger_request_detail', 'd')
    ->fields('d', array('factorName'))
    ->condition('factorName', 'IPViking Category Factor', '<>')
    ->condition('disposition', 0)
    ->distinct()
    ->execute()
    ->fetchCol();
  $form['table-filter-wrapper']['table-filter']['category-filter'] = array(
    '#type' => 'select',
    '#title' => t('Category'),
    '#options' => drupal_map_assoc(array_values(array_merge($category_names, $factor_names))),
    '#empty_option' => t('All'),
    '#empty_value' => 'all',
    '#ajax' => array(
      'callback' => 'ipvenger_control_center_table_callback',
      'wrapper' => 'ipvenger-table',
    ),
    '#attributes' => array('class' => array('ipvenger-table-filter')),
  );
  $form['table-filter-wrapper']['table-filter']['appeal'] = array(
    '#type' => 'checkbox',
    '#title' => t('Appeals Only'),
    '#ajax' => array(
      'callback' => 'ipvenger_control_center_table_callback',
      'wrapper' => 'ipvenger-table',
    ),
    '#attributes' => array('class' => array('ipvenger-table-filter')),
  );
  // Empty form submit will clear all values on a page.
  $form['table-filter-wrapper']['table-filter']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Clear Filters'),
    '#attributes' => array('class' => array('ipvenger-table-filter')),
  );
  $form['table'] = array(
    'table' => array(
      '#theme' => 'table',
      '#header' => array(
        'ipAddress' => array(
          'data' => t('IP ADDRESS'),
        ),
        'timestamp' => array(
          'data' => t('TIMESTAMP'),
        ),
        'country' => array(
          'data' => t('COUNTRY'),
        ),
        'organization' => array(
          'data' => t('ORGANIZATION'),
        ),
        'riskFactor' => array(
          'data' => t('IPQ'),
        ),
        'dispositionReason' => array(
          'data' => t('DISPOSITION'),
        ),
        'categoryName' => array(
          'data' => t('CATEGORY'),
        ),
        'appealId' => array(
          'data' => t('APPEAL'),
        ),
        'whiteBlacklist' => array(
          'data' => t('WHITE/BLACKLIST'),
        ),
      ),
      '#rows' => $rows,
      '#sticky' => TRUE,
      '#empty' => t('No results. Please expand the filter.'),
    ),
    'pager' => array(
      '#theme' => 'pager',
    ),
    // The prefix/suffix provide the div that we're replacing, named by
    // #ajax['wrapper'] in the form filters above.
    '#prefix' => '<div id="ipvenger-table">',
    '#suffix' => '</div>',
  );

  // Dummy div for AJAX dialog.
  $form['dialog'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('ipvenger-dialog')),
  );

  // Add javascript for ip address detail dialog window.
  $form['#attached']['library'][] = array('system', 'ui.dialog');
  $form['#attached']['library'][] = array('system', 'ui.datepicker');
  $form['#attached']['js'] = array(
    drupal_get_path('module', 'ipvenger') . '/js/ipvenger.js' => array(
      'type' => 'file',
    ),
  );
  // Attaching this here so the css can be used by the ip-history-lookup modal.
  $form['#attached']['css'] = array(
    drupal_get_path('module', 'ipvenger') . '/css/ipvenger.css' => array(
      'type' => 'file',
    ),
  );

  return $form;
}

/**
 * Callback element need only select the portion of the form to be updated.
 *
 * @return array
 *   renderable array
 */
function ipvenger_control_center_table_callback($form, &$form_state) {
  return $form['table'];
}

/**
 * Page callback to generate html for viewing history of a given IP address.
 *
 * @param string $ip_address
 *   The ip address to build a history report.
 *
 * @return string
 *   The html content to output.
 */
function ipvenger_ip_history_lookup($ip_address) {
  $render = array();

  if (!empty($ip_address)) {
    $header = array(
      'timestamp' => array(
        'data' => t('TIMESTAMP'),
      ),
      'riskFactor' => array(
        'data' => t('IPQ'),
      ),
      'disposition' => array(
        'data' => t('DISPOSITION'),
      ),
      'dispositionReason' => array(
        'data' => t('REASON'),
      ),
      'categoryName' => array(
        'data' => t('CATEGORY'),
      ),
    );
    $query = db_select('ipvenger_request_detail', 'd')
      ->condition('ipAddress', $ip_address);
    $count = $query
      ->countQuery()
      ->execute()
      ->fetchField();
    $result = $query
      ->fields('d',
        array(
          'timestamp',
          'riskFactor',
          'disposition',
          'dispositionReason',
          'factorName',
          'categoryName',
          'country',
          'city',
          'organization',
        )
      )
      ->extend('PagerDefault')
      ->limit(10)
      ->orderBy('timestamp', 'DESC')
      ->execute()
      ->fetchAll(PDO::FETCH_ASSOC);

    $render['ip-history-header-wrapper'] = array(
      '#type' => 'fieldset',
      '#attributes' => array(
        'class' => array('ipvenger'),
      ),
    );
    $render['ip-history-header-wrapper']['ip-history-details'] = array(
      '#type' => 'fieldset',
      '#attributes' => array(
        'class' => array('ipvenger', 'ipvenger-left'),
      ),
    );
    $render['ip-history-header-wrapper']['ip-exception-list'] = array(
      '#type' => 'fieldset',
      '#attributes' => array(
        'class' => array('ipvenger', 'ipvenger-right'),
      ),
    );
    $country = isset($result[0]['country']) ? $result[0]['country'] : NULL;
    $city = isset($result[0]['city']) ? $result[0]['city'] : NULL;
    $organization = isset($result[0]['organization']) ? $result[0]['organization'] : NULL;
    $render['ip-history-header-wrapper']['ip-history-details']['country'] = array(
      '#markup' => '<div>' . t('Country:') . ' ' . $country . '</div>');
    $render['ip-history-header-wrapper']['ip-history-details']['city'] = array(
      '#markup' => '<div>' . t('City:') . ' ' . $city . '</div>');
    $render['ip-history-header-wrapper']['ip-history-details']['organization'] = array(
      '#markup' => '<div>' . t('Organization:') . ' ' . $organization . '</div>');
    $render['ip-history-header-wrapper']['ip-history-details']['count'] = array(
      '#markup' => '<div>' . t('Records found:') . ' ' . $count . '</div>');

    if ($count) {
      $render['ip-history-header-wrapper']['ip-exception-list']['select-title'] = array(
        '#markup' => '<label>' . t('Protection') . '</label>',
      );
      $render['ip-history-header-wrapper']['ip-exception-list']['select'] = array('#markup' => _ipvenger_ip_exception_select_list($ip_address));
    }

    $rows = array_map('_ipvenger_ip_history_lookup_row_callback', $result);

    $render['history_table'] = array(
      'table' => array(
        '#theme' => 'table',
        '#header' => $header,
        '#rows' => $rows,
        '#sticky' => TRUE,
        '#empty' => t('No requests found for @ip_address',
          array('@ip_address' => $ip_address)),
      ),
      'pager' => array(
        '#theme' => 'pager',
      ),
    );

    $render['#attached']['js'] = array(
      drupal_get_path('module', 'ipvenger') . '/js/ipvenger.js' => array(
        'type' => 'file',
      ),
    );
    $render['#attached']['css'] = array(
      drupal_get_path('module', 'ipvenger') . '/css/ipvenger.css' => array(
        'type' => 'file',
      ),
    );
  }

  return drupal_render($render);
}

/**
 * Page callback for adding/removing a country/IP address black/whitelist entry.
 */
function ipvenger_mask_action($action, $mask) {
  // No need to validate $action as the switch statement does that.
  if (_ipvenger_ip_address_validator($mask)) {
    switch ($action) {
      case 'allow':
        _ipvenger_merge_ip_address_exception($action, $mask);
        break;

      case 'deny':
        _ipvenger_merge_ip_address_exception($action, $mask);
        break;

      case 'protect':
        db_delete('ipvenger_exception')
          ->condition('mask', $mask)
          ->execute();
        break;
    }
  }
  else {
    switch ($action) {
      case 'deny':
        db_insert('ipvenger_exception')
          ->fields(
            array(
              'action' => 'deny',
              'mask' => $mask,
              'exceptionType' => 'country',
            )
          )->execute();
        break;

      case 'protect':
        db_delete('ipvenger_exception')
          ->condition('mask', $mask)
          ->execute();
        break;
    }
  }
}

/**
 * Helper function to populate the risk threshold value.
 *
 * @param string $key
 *   This will determine which value is returned.
 *
 * @return int
 *   numeric risk threshold
 */
function _ipvenger_risk_threshold_value($key = '') {
  $options = array(
    'marketing' => 54,
    'social_platform' => 52,
    'corporate' => 50,
    'blog' => 48,
    'web_app' => 40,
    'ecommerce' => 33,
  );
  if (isset($options[$key])) {
    return $options[$key];
  }
  else {
    // The default site function.
    return '42';
  }
}

/**x
 * Helper function to populate the risk threshold description.
 *
 * @param string $key
 *   This will determine which description is returned.
 *
 * @return string
 *   Risk threshold description.
 */
function _ipvenger_risk_threshold_description($key = '') {
  $options = array(
    'marketing' => t('You will block traffic that is shown to have: extreme risk indicated due to risk category activities seen in past 24 hours, extreme prior risk category activities seen, extreme IP address risk behavior seen.'),
    'social_platform' => t('You will block traffic that is shown to have: extreme risk indicated due to risk category activities seen in past 24 hours, extreme prior risk category activities seen, extreme IP address risk behavior seen.'),
    'corporate' => t('You will block traffic that is shown to have: extreme risk indicated due to risk category activities seen in past 24 hours, extreme prior risk category activities seen, extreme IP address risk behavior seen.'),
    'blog' => t('You will block traffic that is shown to have: extreme risk indicated due to risk category activities seen in past 24 hours, extreme prior risk category activities seen, extreme IP address risk behavior seen.'),
    'web_app' => t('You will block traffic that is shown to have: significant risk characteristics seen in past 24 hours, significant prior risky characteristics seen, significant IP address irregularities seen.'),
    'ecommerce' => t('You will block traffic that is shown to have: some risky characteristics seen in past 24 hours, some prior risky characteristics seen, some IP address irregularities seen.'),
    'custom' => t('The default settings for each site type are carefully calculated to maximize security while minimizing impact on legitimate users. Raising the blocking threshold for your site may allow dangerous IPs to access your site. Lowering the blocking threshold may restrict users that do not pose a significant risk.'),
  );
  if (isset($options[$key])) {
    return $options[$key];
  }
  else {
    return t('You will block traffic that is shown to have: significant risk characteristics seen in past 24 hours, significant prior risky characteristics seen, significant IP address irregularities seen.');
  }
}

/**
 * Helper function that auto detects the mask type.
 *
 * Currently the only wildcards allowed are simplified regex strings
 * limited to "*" wildcards, so just see if the mask contains any asterisks.
 *
 * @param string $mask
 *   IP address mask.
 *
 * @return string
 *   Either 'exact' or 'wildcard'.
 */
function _ipvenger_mask_type($mask) {
  if (strpos($mask, '*') === FALSE) {
    $mask_type = 'exact';
  }
  else {
    $mask_type = 'wildcard';
  }
  return $mask_type;
}

/**
 * Helper function to render a select list for white/blacklist of an IP address.
 *
 * @param string $ip_address
 *   The ip address for which to build the select list.
 *
 * @return string
 *   The html content to output.
 */
function _ipvenger_ip_exception_select_list($ip_address) {
  if (user_access('administer ipvenger')) {
    $result = db_query('SELECT action FROM {ipvenger_exception} WHERE mask = :ip_address', array(':ip_address' => $ip_address));
    $default_value = $result ? $result->fetchField() : 'protect';
    $render = array(
      '#type' => 'select',
      '#options' => array(
        'protect' => t('Protected'),
        'deny' => t('Blacklisted'),
        'allow' => t('Whitelisted'),
      ),
      '#value' => $default_value,
      '#attributes' => array(
        'class' => array(
          str_replace('.', '-', $ip_address),
          'ipvenger',
        ),
      ),
    );

    return drupal_render($render);
  }
}

/**
 * Helper function to validate an IP address.
 *
 * @param string $ip_address
 *   The ip address to validate.
 *
 * @return bool
 *   FALSE if the IP address doesn't validate, otherwise TRUE.
 */
function _ipvenger_ip_address_validator($ip_address) {
  // We need to validate for wildcard (*).
  $ip_address_parts = explode('.', $ip_address);
  foreach ($ip_address_parts as $part) {
    if (!ctype_digit($part) && $part != '*') {
      return FALSE;
    }
  }
  $ip_address = str_replace('*', 1, $ip_address);

  return filter_var($ip_address, FILTER_VALIDATE_IP);
}

/**
 * Helper function to merge an IP address exception into the exception table.
 *
 * It also cleans up existing entries in the appeals table.
 *
 * @param string $action
 *   The action of allow or deny access for an IP address.
 * @param string $ip_address
 *   The IP address to merge into the exception table.
 */
function _ipvenger_merge_ip_address_exception($action, $ip_address) {
  $mask_type = _ipvenger_mask_type($ip_address);
  // Merge the ip address into the db.
  db_merge('ipvenger_exception')
    ->key(array(
      'mask' => $ip_address,
      'exceptionType' => 'ip',
      'maskType' => $mask_type,
    ))
    ->fields(array(
      'mask' => $ip_address,
      'exceptionType' => 'ip',
      'action' => $action,
      'maskType' => $mask_type,
    ))
    ->execute();
  // For exact match ip rules, delete any pending appeals.
  if ($mask_type == 'exact') {
    db_delete('ipvenger_appeal')
      ->condition('ipAddress', $ip_address)
      ->execute();
  }
}

/**
 * Helper function to map rows from the database.
 *
 * @param array $row
 *   The row to map.
 *
 * @return array
 *   A mapped row.
 */
function _ipvenger_admin_control_center_row_callback($row) {
  // We need a css class for styling.
  $row['ipAddress'] = array(
    'data' => $row['ipAddress'],
    'class' => 'ipvenger-ip-address',
  );

  // Render timestamp as a date format.
  $row['timestamp'] = format_date($row['timestamp'], 'short');

  // If the factor is an IPVenger custom factor,
  // then use the category name for the factor.
  if ($row['factorName'] == 'IPViking Category Factor') {
    $row['factorName'] = $row['categoryName'];
  }

  // Remove categoryName since it isn't needed.
  unset($row['categoryName']);

  // Change appeal result into Y.
  $row['appealId'] = !empty($row['appealId']) ? 'Y' : NULL;

  // Fabricate the whitelist/blacklist row since it doesn't exist in the db.
  $row['whiteBlacklist'] = _ipvenger_ip_exception_select_list($row['ipAddress']['data']);

  return (array) $row;
}

/**
 * Helper function to map rows from the database.
 *
 * @param array $row
 *   The row to map.
 *
 * @return array
 *   A mapped row.
 */
function _ipvenger_ip_history_lookup_row_callback($row) {
  // Render timestamp as a date format.
  $row['timestamp'] = format_date($row['timestamp'], 'short');

  // Disposition of '0' means rejected.
  if ($row['disposition'] == 0) {
    $row['disposition'] = t('Rejected');
  }

  // If the factor is an IPVenger custom factor,
  // then use the category name for the factor.
  if ($row['factorName'] == 'IPViking Category Factor') {
    $row['factorName'] = $row['categoryName'];
  }

  // Remove several rows since they aren't needed.
  unset($row['country']);
  unset($row['city']);
  unset($row['organization']);
  unset($row['categoryName']);

  return (array) $row;
}

/**
 * Export submit handler for ipvenger_admin_advanced_settings().
 */
function _ipvenger_data_export_submit($form, &$form_state) {
  $start_date = $form_state['values']['ipvenger-data-export'];

  drupal_add_http_header('Cache-Control', 'must-revalidate, post-check=0, pre-check=0');
  drupal_add_http_header('Content-Type', 'text/csv');
  drupal_add_http_header('Content-Disposition', 'attachment; filename=' . 'ipvenger-request-data.csv');
  $header = array();
  $values = array();

  $query = db_select('ipvenger_request_detail', 'd');
  $query->fields('d');
  $query->condition('d.timestamp', REQUEST_TIME - $start_date, '>');
  $query->condition('d.timestamp', REQUEST_TIME, '<');
  $result = $query->execute();

  $record = $result->fetchAssoc();
  foreach ($record as $field_name => $field_value) {
    $header[] = $field_name;
    $values[] = $field_value;
  }
  print implode(',', $header) . PHP_EOL;
  print implode(',', $values) . PHP_EOL;

  while ($row = $result->fetchAssoc()) {
    $values = array();
    foreach ($row as $value) {
      $values[] = $value;
    }
    print implode(',', $values) . PHP_EOL;
  }
  module_invoke_all('exit');
  exit;
}
