<?php
/**
 * @file
 * Administrative page callbacks for the IP Venger Report module.
 */

use IPVenger\Service\IPVengerSettings;

/**
 * Generate a form for viewing IP Venger reports.
 */
function ipvenger_reports($form, &$form_state) {

  // Dummy div for AJAX dialog.
  $form['dialog'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('ipvenger-dialog')),
  );

  $query = drupal_get_query_parameters();
  if (isset($query['days']) && ctype_digit($query['days']) && $query['days'] > 0 && $query['days'] < 31) {
    $days = $query['days'];
    $start_date  = strtotime($days . ' days ago');
  }
  elseif (isset($form_state['values']['date-range'])) {
    $days = $form_state['values']['date-range'];
    $start_date  = strtotime($days . ' days ago');
  }
  else {
    $days = 7;
    $start_date = strtotime(7 . ' days ago');
  }

  $form['date-range'] = array(
    '#type' => 'select',
    '#options' => array(
      7 => t('Last 7 days'),
      14 => t('Last 14 days'),
      30 => t('Last 30 days'),
    ),
    '#description' => t('View report for !start_date - !end_date',
      array(
        '!start_date' => format_date($start_date, 'custom', 'F j, Y'),
        '!end_date' => format_date(REQUEST_TIME, 'custom', 'F j, Y'),
      )
    ),
    '#default_value' => $days,
    '#attributes' => array('onchange' => 'this.form.submit();'),
  );

  // Build HTML form structure.
  $form['analytics-wrapper'] = array(
    '#type' => 'container',
    '#attributes' => array('id' => array('ipvenger-analytics-wrapper')),
  );
  $form['analytics-wrapper']['date-range'] = array();
  $form['analytics-wrapper']['summary-total'] = array();

  $form['analytics-wrapper']['summary'] = array(
    '#type' => 'container',
  );
  $form['analytics-wrapper']['summary']['disposition'] = array(
    '#type' => 'container',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper'] = array(
    '#type' => 'container',
  );
  $form['analytics-wrapper']['summary']['category'] = array(
    '#type' => 'container',
  );
  $form['analytics-wrapper']['summary']['category']['category-wrapper'] = array(
    '#type' => 'container',
  );
  $form['analytics-wrapper']['summary']['category']['category-wrapper']['top']['graph'] = array(
    '#type' => 'container',
  );
  $form['analytics-wrapper']['summary']['category']['category-wrapper']['top']['category-graph-legend-wrapper'] = array(
    '#type' => 'container',
  );
  $form['analytics-wrapper']['summary']['category']['category-wrapper']['bottom'] = array(
    '#type' => 'container',
  );
  $form['analytics-wrapper']['daily-graph'] = array(
    '#type' => 'container',
  );
  $form['analytics-wrapper']['country-graph'] = array(
    '#type' => 'container',
  );
  $form['analytics-wrapper']['country-table'] = array(
    '#type' => 'container',
  );
  $form['ip-ticker'] = array(
    '#type' => 'container',
  );

  // Totals.
  $query = db_select('ipvenger_request_detail', 'd');
  $query->condition('d.timestamp', $start_date, '>');
  $results['total_count'] = $query->countQuery()->execute()->fetchField();
  $query = db_select('ipvenger_request_detail', 'd');
  $query->condition('d.timestamp', $start_date, '>');
  $query->condition('d.disposition', 0);
  $results['blocked_count'] = $query->countQuery()->execute()->fetchField();

  // Allowed-Blocked IPs.
  $query = db_select('ipvenger_request_detail', 'd');
  $query->addField('d', 'dispositionReason', 'label');
  $query->addExpression('COUNT(dispositionReason)', 'data');
  $query->condition('d.disposition', 0);
  $query->condition('d.timestamp', $start_date, '>');
  $query->groupBy('label');
  $union = db_select('ipvenger_request_detail', 'd');
  $union->addExpression('\'Allowed\'', 'label');
  $union->addExpression('COUNT(dispositionReason)', 'data');
  $union->condition('d.disposition', 1);
  $union->condition('d.timestamp', $start_date, '>');
  $union->groupBy('label');
  $query->union($union);
  $results['allowed-blocked'] = $query->execute()->fetchAllAssoc('label');

  // Factors.
  $query = db_select('ipvenger_request_detail', 'd');
  $query->addField('d', 'factorName', 'label');
  $query->addExpression('COUNT(factorName)', 'data');
  $query->condition('d.disposition', 0);
  $query->condition('d.dispositionReason', 'IPQ Score');
  $query->condition('d.factorName', 'IPViking Category Factor', '<>');
  $query->condition('d.timestamp', $start_date, '>');
  $query->groupBy('label');

  $union = db_select('ipvenger_request_detail', 'd');
  $union->addField('d', 'categoryName', 'label');
  $union->addExpression('COUNT(categoryName)', 'data');
  $union->condition('d.disposition', 0);
  $union->condition('d.dispositionReason', 'IPQ Score');
  $union->condition('d.factorName', 'IPViking Category Factor');
  $union->condition('d.timestamp', $start_date, '>');
  $union->groupBy('label');
  $query->union($union);
  $results['factors'] = $query->execute()->fetchAllKeyed();

  // Daily Risk.
  $query = db_select('ipvenger_request_detail', 'd');
  $query->addExpression(strtotime('midnight'), 'date');
  $query->addExpression('COUNT(timestamp)', 'count');
  $query->condition('d.disposition', 0);
  $query->condition('d.timestamp', strtotime('now'), '<');
  $query->condition('d.timestamp', strtotime('midnight today'), '>=');
  $query->condition('d.dispositionReason', 'IPQ Score');

  for ($i = 1; $i < $days;) {
    $union = db_select('ipvenger_request_detail', 'd');
    $union->addExpression(strtotime('midnight ' . $i . 'days ago'), 'date');
    $union->addExpression('COUNT(timestamp)', 'count');
    $union->condition('d.disposition', 0);
    $union->condition('d.timestamp', strtotime('midnight ' . $i . ' day ago'), '<');
    $union->condition('d.timestamp', strtotime('midnight ' . ++$i . ' days ago'), '>=');
    $union->condition('d.dispositionReason', 'IPQ Score');
    $query->union($union);
  }
  $results['daily-risk']['IPQ Score'] = $query->execute()->fetchAll();

  $query = db_select('ipvenger_request_detail', 'd');
  $query->addExpression(strtotime('midnight'), 'date');
  $query->addExpression('COUNT(timestamp)', 'count');
  $query->condition('d.disposition', 0);
  $query->condition('d.timestamp', strtotime('now'), '<');
  $query->condition('d.timestamp', strtotime('midnight today'), '>=');
  $query->condition('d.dispositionReason', 'IP Blacklist');

  for ($i = 1; $i < $days;) {
    $union = db_select('ipvenger_request_detail', 'd');
    $union->addExpression(strtotime('midnight ' . $i . 'days ago'), 'date');
    $union->addExpression('COUNT(timestamp)', 'count');
    $union->condition('d.disposition', 0);
    $union->condition('d.timestamp', strtotime('midnight ' . $i . ' day ago'), '<');
    $union->condition('d.timestamp', strtotime('midnight ' . ++$i . ' days ago'), '>=');
    $union->condition('d.dispositionReason', 'IP Blacklist');
    $query->union($union);
  }
  $results['daily-risk']['IP Blacklist'] = $query->execute()->fetchAll();

  $query = db_select('ipvenger_request_detail', 'd');
  $query->addExpression(strtotime('midnight'), 'date');
  $query->addExpression('COUNT(timestamp)', 'count');
  $query->condition('d.disposition', 0);
  $query->condition('d.timestamp', strtotime('now'), '<');
  $query->condition('d.timestamp', strtotime('midnight today'), '>=');
  $query->condition('d.dispositionReason', 'Country Blacklist');

  for ($i = 1; $i < $days;) {
    $union = db_select('ipvenger_request_detail', 'd');
    $union->addExpression(strtotime('midnight ' . $i . 'days ago'), 'date');
    $union->addExpression('COUNT(timestamp)', 'count');
    $union->condition('d.disposition', 0);
    $union->condition('d.timestamp', strtotime('midnight ' . $i . ' day ago'), '<');
    $union->condition('d.timestamp', strtotime('midnight ' . ++$i . ' days ago'), '>=');
    $union->condition('d.dispositionReason', 'Country Blacklist');
    $query->union($union);
  }
  $results['daily-risk']['Country Blacklist'] = $query->execute()->fetchAll();

  // Country graph data.
  $country_sorts = array('percentage', 'average_score');
  foreach ($country_sorts as $sort) {
    $query = db_select('ipvenger_request_detail', 'd');
    $query->addField('d', 'country', 'label');
    $query->addExpression('AVG(riskFactor)', 'average_score');
    $query->addExpression('SUM(NOT disposition)/COUNT(*) * 100', 'percentage');
    $query->condition('d.timestamp', $start_date, '>');
    $query->groupBy('label');
    $query->orderBy($sort, 'DESC');
    $query->range(0, 5);
    $results['country'][$sort] = $query->execute()->fetchAll();
  }

  // Country detail table.
  $query = db_select('ipvenger_request_detail', 'd')->extend('PagerDefault')->extend('TableSort');
  $query->addField('d', 'country', 'country');
  $query->addExpression('SUM(NOT disposition)/COUNT(*) * 100', 'blockedPercentage');
  $query->addExpression('COUNT(*) * 100' . '/' . $results['total_count'], 'countryPercentage');
  $query->addExpression('AVG(riskFactor)', 'averageScore');
  $query->leftJoin('ipvenger_exception', 'e', '(e.mask = d.country)');
  $query->addField('e', 'action', 'protection');
  $query->condition('d.timestamp', $start_date, '>');
  $query->orderBy('country');
  $query->groupBy('d.country');

  // Optionally add a filter by selected country.
  if (isset($form_state['values']['country'])) {
    $country = $form_state['values']['country'];
    if ($country != 'all') {
      $query->condition('country', $country);
    }
  }

  $results['country_detail'] = $query->execute()->fetchAll(PDO::FETCH_ASSOC);

  $form['analytics-wrapper']['summary-total'] = array(
    '#markup' => '<div class="graph-title">' . t('IPVenger has blocked') . ' ' . '<span class="emphasis">' . $results['blocked_count'] . ' (' . ($results['total_count'] == 0 ? '0' : number_format($results['blocked_count'] / $results['total_count'], 2)) . '%)</span> ' . t('out of') . ' ' . '<span class="emphasis">' . $results['total_count'] . ' ' . t('visits') . '</span> ' . t('to your website.') . '</div>',
  );

  // Render the disposition pie chart.
  $label_colors = array(
    'IPQ Score' => '#CC1A00',
    'Country Blacklist' => '#F6821F',
    'IP Blacklist' => '#999999',
    'Allowed' => '#00ADDE',
  );
  $datas = array();
  foreach ($label_colors as $label => $color) {
    $data = isset($results['allowed-blocked'][$label]) ? $results['allowed-blocked'][$label]->data : 0;
    $flot_data = new stdClass();
    $flot_data->data = (int) $data;
    $flot_data->label = $label;
    $flot_data->color = $color;
    switch ($label) {
      case 'IPQ Score':
        $flot_data->label = t('Failed IPQ Score');
        break;

      case 'Country Blacklist':
        $flot_data->label = t('Country blocked');
        break;

      case 'IP Blacklist':
        $flot_data->label = t('IP Blacklisted');
        break;

      case 'Allowed':
        $flot_data->label = t('Accepted Traffic');
        break;
    }
    $datas[] = $flot_data;
  }

  $disposition_options = new stdClass();
  $disposition_options->series->pie->show = TRUE;
  $disposition_options->series->pie->radius = 0.95;
  $disposition_options->series->pie->stroke->width = 3;
  $disposition_options->series->pie->label->show = FALSE;
  $disposition_options->grid->hoverable = TRUE;
  $disposition_options->legend->show = FALSE;
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['graph'] = array(
    '#theme' => 'flot_graph',
    '#data' => $datas,
    '#element' => array(
      'id' => 'ipvenger-reports-disposition-pie',
      'class' => 'pie',
      'style' => 'width:214px;height:214px',
    ),
    '#options' => $disposition_options,
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper'] = array(
    '#type' => 'container',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-visits'] = array(
    '#markup' => '<div id="ipvenger-reports-legend-visits">' . $results['total_count'] . ' ' . t('Visitors') . '</div>',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-ipq'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('legend-ipq')),
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-ipq']['ipq-count'] = array(
    '#markup' => '<div class="disposition-legend-count">' . $datas[0]->data . '</div>',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-ipq']['ipq-label'] = array(
    '#markup' => '<div class="disposition-legend-label">' . t('Failed IPQ Score') . '</div>',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-country'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('legend-country')),
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-country']['country-count'] = array(
    '#markup' => '<div class="disposition-legend-count">' . $datas[1]->data . '</div>',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-country']['country-label'] = array(
    '#markup' => '<div class="disposition-legend-label">' . t('Country blocked') . '</div>',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-ip'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('legend-ip')),
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-ip']['ip-count'] = array(
    '#markup' => '<div class="disposition-legend-count">' . $datas[2]->data . '</div>',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-ip']['ip-label'] = array(
    '#markup' => '<div class="disposition-legend-label">' . t('IP Blacklisted') . '</div>',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-allowed'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('legend-allowed')),
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-allowed']['allowed-count'] = array(
    '#markup' => '<div class="disposition-legend-count">' . $datas[3]->data . '</div>',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-allowed']['allowed-label'] = array(
    '#markup' => '<div class="disposition-legend-label">' . t('Accepted Traffic') . '</div>',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['reason-text'] = array(
    '#type' => 'item',
    '#title' => t('The reason they were blocked')
    . '<span class="ivenger-tooltip" title="' . t('After IPVenger determines whether to allow or reject a request to your site, it stores the disposition of that request for future analysis. Blocked requests are always assigned one of three dispositions; IPQ Score, IP Blacklist or Country Blacklist. If the disposition for a request is IPQ Score, this means that the request was rejected because it came from an IP Address with an IPQ score higher than the maximum allowed for your site. The Blacklist dispositions are assigned to requests from IP addresses or countries the user has explicitly blocked. This chart shows the proportion of these dispositions across the entire specified date range.') . '"> [?]</span>',
    '#markup' => '<div id="disposition-reason-text">' . t('The pie chart breaks down all visitors to your site, according to the action taking by IPVenger. The bar graph below shows rejected requests by type on a daily basis.') . '</div>',
  );

  // Render the category pie chart.
  // We need to make sure there are only four values to chart.
  $factors = $results['factors'];
  asort($factors);
  $factor_one = _ipvenger_reports_pop_key_value($factors);
  $factor_two = _ipvenger_reports_pop_key_value($factors);
  $factor_three = _ipvenger_reports_pop_key_value($factors);
  $factors_others['Other'] = array_sum($factors);
  $factors = array_merge($factor_one, $factor_two, $factor_three, $factors_others);

  $factor_colors = array(
    '#660000',
    '#CC0000',
    '#FF5500',
    '#FF9933',
  );

  $category_options = new stdClass();
  $category_options->series->pie->show = TRUE;
  $category_options->series->pie->radius = 0.95;
  $category_options->series->pie->stroke->width = 3;
  $category_options->series->pie->label->show = FALSE;
  $category_options->grid->hoverable = TRUE;
  $category_options->legend->show = FALSE;
  $category_options->legend->labelBoxBorderColor = '#FFF';
  $category_options->legend->labelFormatter = 'fn:Drupal.ipvenger_reports.pieLabelFormatter';
  $category_options->legend->container = '#edit-category-graph-legend';

  $datas = array();
  $iterator = 0;

  $rows = array();
  foreach ($factors as $label => $data) {
    $flot_data = new stdClass();
    $flot_data->data = (int) $data;
    $flot_data->label = $label;
    $flot_data->color = $factor_colors[$iterator];

    $datas[] = $flot_data;

    $rows[] = array(
      'data' => array(
        array(
          'data' => '<div class="category-legend-count">' . $data . '</div>' . $label ,
          'style' => array('color:' . $flot_data->color),
          'class' => array('legend-label'),
        ),
      ),
      'no_striping' => TRUE,
    );

    ++$iterator;
  }

  $form['analytics-wrapper']['summary']['category']['category-wrapper']['top']['graph'] = array(
    '#theme' => 'flot_graph',
    '#data' => $datas,
    '#element' => array(
      'id' => 'ipvenger-reports-category-pie',
      'class' => 'pie',
      'style' => 'width:140px;height:140px',
    ),
    '#options' => $category_options,
  );
  $form['analytics-wrapper']['summary']['category']['category-wrapper']['top']['category-graph-legend-wrapper']['category-graph-legend-title'] = array(
    '#markup' => '<div id="category-pie-legend-title">IPQ Breakdown</div>',
  );
  $form['analytics-wrapper']['summary']['category']['category-wrapper']['top']['category-graph-legend-wrapper']['category-graph-legend'] = array(
    '#type' => 'container',
  );
  $form['analytics-wrapper']['summary']['category']['category-wrapper']['top']['category-graph-legend-wrapper']['category-graph-legend']['category-graph-legend-table'] = array(
    '#theme' => 'table',
    '#rows' => $rows,
    '#empty' => t('No results yet.'),
    '#attributes' => array('id' => 'category-pie-legend'),
  );
  $form['analytics-wrapper']['summary']['category']['category-wrapper']['bottom']['category-reason-text'] = array(
    '#type' => 'item',
    '#title' => t('The main threat categories')
    . '<span class="ivenger-tooltip" title="' . t('Although hundreds of individual data points contribute to the IPQ Score, it is useful to see an overview of why an IP Address has been blocked. When IPVenger blocks an IP address from accessing your site, it stores the name of the single data point that had the biggest negative effect on the IPQ score. This is the Category associated with the request.')
    . t('This pie chart shows the proportion of each of the categories for requests that were blocked during the indicated date range. Categories that represent less than 1% of the total blocks are summarized as Other. Detailed information on each of the category types can be found on the IPVenger website.') . '"> [?]</span>',
    '#markup' => '<div>' . t('This chart applies to requests that where rejected due to a high IPQ score. It shows the main reason that the IPQ score was too high.') . '</div>',
  );

  // Render the daily blocks bar chart.
  $datas = array();
  $flot_data = new stdClass();
  $flot_data->label = t('IPQ Score');
  $flot_data->color = '#CC1A00';
  foreach ($results['daily-risk']['IPQ Score'] as $delta => $result) {
    $flot_data->data[$delta] = array((int) $result->date * 1000, (int) $result->count);
  }
  $datas[] = $flot_data;
  $flot_data = new stdClass();
  $flot_data->label = t('IP Blacklist');
  $flot_data->color = '#999999';
  foreach ($results['daily-risk']['IP Blacklist'] as $delta => $result) {
    $flot_data->data[$delta] = array((int) $result->date * 1000, (int) $result->count);
  }
  $datas[] = $flot_data;
  $flot_data = new stdClass();
  $flot_data->label = t('Country Blacklist');
  $flot_data->color = '#F6821F';
  foreach ($results['daily-risk']['Country Blacklist'] as $delta => $result) {
    $flot_data->data[$delta] = array((int) $result->date * 1000, (int) $result->count);
  }
  $datas[] = $flot_data;

  $daily_risk_options = new stdClass();
  $daily_risk_options->grid->borderWidth = 0;
  $daily_risk_options->grid->hoverable = TRUE;
  $daily_risk_options->legend->show = FALSE;
  $daily_risk_options->series->stack = TRUE;
  $daily_risk_options->series->bars->show = TRUE;
  $daily_risk_options->series->bars->align = 'center';
  $daily_risk_options->series->bars->barWidth = 18 * 60 * 60 * 1000;
  $daily_risk_options->series->bars->fill = .75;
  $daily_risk_options->series->bars->lineWidth = 0;
  $daily_risk_options->xaxis->mode = 'time';
  $daily_risk_options->xaxis->tickLength = 0;
  $daily_risk_options->xaxis->minTickSize = array(1, 'day');
  $daily_risk_options->xaxis->timeformat = '%m/%d';
  $daily_risk_options->yaxis->tickDecimals = 0;

  $form['analytics-wrapper']['daily-graph']['daily-graph-title'] = array(
    '#type' => 'item',
    '#title' => t('Daily blocks by type')
    . '<span class="ivenger-tooltip" title="' . t('The height of each bar represents the total number of requests that were blocked on a particular day. Each bar is subdivided to show the number of blocks for each reason (IPQ Score, IP Blacklist and IPQ Score).') . '"> [?]</span>',
  );
  $form['analytics-wrapper']['daily-graph']['daily-y-label'] = array(
    '#type' => 'item',
    '#description' => t('Number blocked'),
  );
  $form['analytics-wrapper']['daily-graph']['daily-graph-wrapper'] = array(
    '#type' => 'container',
  );
  $form['analytics-wrapper']['daily-graph']['daily-graph-wrapper']['daily-graph'] = array(
    '#theme' => 'flot_graph',
    '#data' => $datas,
    '#element' => array(
      'id' => 'ipvenger-reports-daily-risk',
      'class' => 'bar',
      'style' => 'width:712px;height:210px',
    ),
    '#options' => $daily_risk_options,
  );
  $form['analytics-wrapper']['daily-graph']['daily-graph-wrapper']['daily-graph-legend'] = array(
    '#type' => 'container',
  );
  $form['analytics-wrapper']['daily-graph']['daily-graph-wrapper']['daily-graph-legend']['ipq'] = array(
    '#markup' => '<div class="legend-color"><div class="legend-color-inner daily-legend-ipq"></div></div>' . '<label>' . t('IPQ Score') . '</label>',
  );
  $form['analytics-wrapper']['daily-graph']['daily-graph-wrapper']['daily-graph-legend']['ip'] = array(
    '#markup' => '<div class="legend-color"><div class="legend-color-inner daily-legend-ip"></div></div>' . '<label>' . t('IP Blacklist') . '</label>',
  );
  $form['analytics-wrapper']['daily-graph']['daily-graph-wrapper']['daily-graph-legend']['country'] = array(
    '#markup' => '<div class="legend-color"><div class="legend-color-inner daily-legend-country"></div></div>' . '<label>' . t('Country Blacklist') . '</label>',
  );
  if (user_access('administer ipvenger')) {
    $form['analytics-wrapper']['daily-graph']['daily-graph-wrapper']['daily-link-container'] = array(
      '#type' => 'item',
      '#markup' => l(t('See the full list of IP addresses blocked'), 'admin/config/security/ipvenger/control-center', array('class' => array('ipv-link-button'))),
    );
  }

  // Render the country bar chart.
  $form['analytics-wrapper']['country-graph']['country-graph-title'] = array(
    '#type' => 'item',
    '#title' => t('Threats by Country of Origin')
    . '<span class="ivenger-tooltip" title="' . t('For each country, the first bar shows the percentage of all blocked requests that originated from that country. The second bar shows the average IPQ score of all requests for the given country (both blocked and allowed). Data for at most fifteen countries is shown in descending order based on the sort option you select. Data for all countries that have generated traffic to your website is displayed in the table below the graph.') . '"> [?]</span>',
  );
  $form['analytics-wrapper']['country-graph']['country-graph-container'] = array(
    '#type' => 'container',
  );
  $form['analytics-wrapper']['country-graph']['country-graph-container']['country-sort-container'] = array(
    '#type' => 'item',
    '#markup' => t('Sort by')
    . '| <span id="ipvenger-country-sort-block">'
    . t('% Blocked by Country')
    . '</span> | <span id="ipvenger-country-sort-ipq">'
    . t('Average IPQ Score')
    . '</span> <div class="fullwidth-bar" id="ipvenger-country-bar"></div>',
  );

  foreach ($results['country'] as $key => $result) {
    $key = str_replace('_', '-', $key);
    $country_options = new stdClass();
    $country_options->grid->borderWidth = 0;
    $country_options->grid->hoverable = TRUE;
    $country_options->legend->show = FALSE;
    $country_options->legend->labelBoxBorderColor = '#FFF';
    $country_options->bars->show = TRUE;
    $country_options->bars->barWidth = 3 * 60 * 60 * 1000;
    $country_options->bars->fill = .75;
    $country_options->bars->lineWidth = 0;
    $country_options->xaxis->autoscaleMargin = .1;
    $country_options->xaxis->mode = 'time';
    $country_options->xaxis->tickFormatter = 'fn:Drupal.ipvenger_reports.countryTickFormatter';
    $country_options->xaxis->tickLength = 0;
    $country_options->xaxis->tickSize = array(1, 'day');
    $country_options->yaxis->max = 100;

    $flot_data_percentage = new stdClass();
    $flot_data_percentage->label = t('Percent of blocked requests');
    $flot_data_percentage->color = '#F79622';
    $flot_data_average_score = new stdClass();
    $flot_data_average_score->label = t('Average IPQ');
    $flot_data_average_score->color = '#CC1A00';
    foreach ($result as $delta => $data) {
      $three_pm = 1000 * ((strtotime('midnight yesterday') + ($delta + 1) * 60 * 60 * 24) + 54000);
      $seven_pm = 1000 * ((strtotime('midnight yesterday') + ($delta + 1) * 60 * 60 * 24) + 68400);
      $six_fifteen_pm = 1000 * ((strtotime('midnight yesterday') + ($delta + 1) * 60 * 60 * 24) + 65700);
      // This is hackery, but an easy way to avoid depending on orderBars.js.
      $country_options->xaxis->ipvenger_reports_country[$seven_pm] = $data->label;
      $flot_data_percentage->data[$delta] = array($three_pm, (int) empty($data->percentage) ? 0 : (int) $data->percentage);
      $flot_data_average_score->data[$delta] = array($six_fifteen_pm, (int) empty($data->average_score) ? 0 : (int) $data->average_score);
    }

    if ($key == 'percentage') {
      $style = array('style' => array(''));
    }
    else {
      $style = array('style' => array('display:none'));
    }
    $form['analytics-wrapper']['country-graph']['country-graph-container']['country-graph-container-' . $key] = array(
      '#type' => 'container',
      '#attributes' => $style,
    );
    $form['analytics-wrapper']['country-graph']['country-graph-container']['country-graph-container-' . $key][$key] = array(
      '#theme' => 'flot_graph',
      '#data' => array($flot_data_percentage, $flot_data_average_score),
      '#element' => array(
        'id' => 'ipvenger-reports-country-graph-' . $key,
        'class' => 'bar',
        'style' => 'width:712px;height:210px',
      ),
      '#options' => $country_options,
    );
  }

  // We only need to load the country list if the ipvenger library is available.
  $library = libraries_load('ipvenger');
  if ($library['installed']) {
    $form['analytics-wrapper']['country-table']['country'] = array(
      '#type' => 'select',
      '#title' => t('Country'),
      '#options' => drupal_map_assoc(IPVengerSettings::getCountryList()),
      '#empty_option' => t('All'),
      '#empty_value' => 'all',
      '#ajax' => array(
        'callback' => '_ipvenger_reports_country_filter',
        'wrapper' => 'country-detail-table',
      ),
    );
  }

  $header = array(
    'country' => array(
      'data' => t('COUNTRY OF ORIGIN'),
      'field' => 'd.country',
      'sort' => 'desc',
    ),
    'blockedPercentage' => array(
      'data' => t('PERCENTAGE ALL BLOCKED'),
    ),
    'countryPercentage' => array(
      'data' => t('PERCENTAGE ALL TRAFFIC'),
    ),
    'averageScore' => array(
      'data' => t('AVERAGE IPQ'),
    ),
    'protection' => array(
      'data' => t('PROTECTION'),
      'field' => 'e.action',
    ),
  );

  $rows = array_map(function ($row) use ($start_date) {
    // We need a css class for styling.
    $country = $row['country'];
    $uri = 'admin/ipvenger/country-details/' . $country . '/' . $start_date;
    $uri = l(t($country), $uri, array('attributes' => array('target' => '_blank')));
    $row['country'] = array(
      'data' => $uri,
      'class' => 'ipvenger-country-name',
    );

    $row['blockedPercentage'] = number_format($row['blockedPercentage'], 2) . '%';
    $row['countryPercentage'] = number_format($row['countryPercentage'], 2) . '%';
    $row['averageScore'] = number_format($row['averageScore'], 2);

    // Change protection result into blacklist/protect.
    $protection = $row['protection'] == 'deny' ? t('Blacklisted') : t('Protected');
    unset($row['protection']);
    $row['protection'] = _ipvenger_reports_country_blacklist_select_list($protection, $country);

    return (array) $row;
  }, $results['country_detail']);

  $form['analytics-wrapper']['country-table']['country-detail-table'] = array(
    'table' => array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#sticky' => TRUE,
      '#empty' => t('No results.'),
    ),
    'pager' => array(
      '#theme' => 'pager',
    ),
    // The prefix/suffix provide the div that we're replacing, named by
    // #ajax['wrapper'] in the form filters above.
    '#prefix' => '<div id="country-detail-table">',
    '#suffix' => '</div>',
  );

  $form['ip-ticker'] = array(
    '#type' => 'item',
    '#title' => t('Recent IPs Blocked'),
  );

  $form['ip-ticker']['container'] = array(
    '#type' => 'container',
    '#attributes' => array('id' => array('ipvenger-reports-recent-blocked')),
  );

  $form['#attached']['library'][] = array('system', 'ui.dialog');
  $form['#attached']['js'] = array(
    drupal_get_path('module', 'ipvenger_reports') . '/js/ipvenger_reports.js' => array('type' => 'file'),
    drupal_get_path('module', 'ipvenger') . '/js/ipvenger.js' => array('type' => 'file'),
  );
  $settings = array('startDate' => $start_date);
  $form['#attached']['js'][] = array(
    'data' => array('ipvenger_reports' => $settings),
    'type' => 'setting',
  );
  $form['#attached']['css'] = array(
    drupal_get_path('module', 'ipvenger_reports') . '/css/ipvenger_reports.css' => array('type' => 'file'),
    drupal_get_path('module', 'ipvenger') . '/css/ipvenger.css' => array('type' => 'file'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Filter by Date'),
    '#attributes' => array('style' => 'display: none;'),
  );

  return $form;
}

/**
 * Generate a form for viewing IP Venger recent IP blocks.
 */
function ipvenger_reports_recent_ip_blocks($form, &$form_state, $timestamp) {
  $query = db_select('ipvenger_request_detail', 'd');
  $query->fields('d', array(
      'ipAddress',
      'timestamp',
      'dispositionReason',
  ));
  $query->condition('d.disposition', 0);
  if (!empty($timestamp) && ctype_digit($timestamp)) {
    $query->condition('d.timestamp', $timestamp, '<');
  }
  $query->orderBy('d.timestamp', 'DESC');
  $query->range(0, 20);
  $results = $query->execute()->fetchAll();

  foreach ($results as $result) {
    $ticker['item'] = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('ticker-row')),
    );
    $ticker['item']['ip-address'] = array(
      '#markup' => '<div class="ipvenger-ip-address pseudo-link">' . $result->ipAddress . '</div>',
    );
    $ticker['item']['date'] = array(
      '#markup' => '<div class="date">' . format_interval(REQUEST_TIME - $result->timestamp, 1) . '</div>',
    );
    $ticker['item']['reason'] = array(
      '#markup' => '<div class="reason">' . $result->dispositionReason . '</div>',
    );
    $form['content'][] = $ticker;
  }

  if (user_access('administer ipvenger')) {
    $form['ticker-footer'] = array(
      '#markup' => l(t('View all blocks'), 'admin/config/security/ipvenger/control-center', array('attributes' => array('class' => 'ticker-bookends'))),

    );
  }

  return $form;
}

/**
 * Page callback for country details.
 */
function ipvenger_reports_country_details($form, &$form_state, $country, $start_date) {
  // Blacklisted country names.
  $query = db_query('SELECT mask FROM {ipvenger_exception} WHERE exceptionType = :type AND action = :action',
      array(':type' => 'country', ':action' => 'deny'));
  $results['blocked_countries'] = $query->fetchCol();

  // Blocked country label details.
  $query = db_select('ipvenger_request_detail', 'd');
  $query->addField('d', 'country', 'label');
  $query->addExpression('AVG(riskFactor)', 'average_score');
  $query->addExpression('SUM(NOT disposition)/COUNT(*) * 100', 'percentage');
  $query->groupBy('label');
  $results['country_labels'] = $query->execute()->fetchAll();

  // Totals.
  $query = db_select('ipvenger_request_detail', 'd');
  $query->condition('d.timestamp', $start_date, '>');
  $query->condition('d.country', $country);
  $results['total_count'] = $query->countQuery()->execute()->fetchField();
  $query = db_select('ipvenger_request_detail', 'd');
  $query->condition('d.timestamp', $start_date, '>');
  $query->condition('d.disposition', 0);
  $query->condition('d.country', $country);
  $results['blocked_count'] = $query->countQuery()->execute()->fetchField();

  // Allowed-Blocked IPs.
  $query = db_select('ipvenger_request_detail', 'd');
  $query->addField('d', 'dispositionReason', 'label');
  $query->addExpression('COUNT(dispositionReason)', 'data');
  $query->condition('d.disposition', 0);
  $query->condition('d.timestamp', $start_date, '>');
  $query->condition('d.country', $country);
  $query->groupBy('label');
  $union = db_select('ipvenger_request_detail', 'd');
  $union->addExpression('\'Allowed\'', 'label');
  $union->addExpression('COUNT(dispositionReason)', 'data');
  $union->condition('d.disposition', 1);
  $union->condition('d.timestamp', $start_date, '>');
  $union->condition('d.country', $country);
  $union->groupBy('label');
  $query->union($union);
  $results['allowed-blocked'] = $query->execute()->fetchAllAssoc('label');

  // Factors.
  $query = db_select('ipvenger_request_detail', 'd');
  $query->addField('d', 'factorName', 'label');
  $query->addExpression('COUNT(factorName)', 'data');
  $query->condition('d.disposition', 0);
  $query->condition('d.dispositionReason', 'IPQ Score');
  $query->condition('d.factorName', 'IPViking Category Factor', '<>');
  $query->condition('d.timestamp', $start_date, '>');
  $query->condition('d.country', $country);
  $query->groupBy('label');

  $union = db_select('ipvenger_request_detail', 'd');
  $union->addField('d', 'categoryName', 'label');
  $union->addExpression('COUNT(categoryName)', 'data');
  $union->condition('d.disposition', 0);
  $union->condition('d.dispositionReason', 'IPQ Score');
  $union->condition('d.factorName', 'IPViking Category Factor');
  $union->condition('d.timestamp', $start_date, '>');
  $union->condition('d.country', $country);
  $union->groupBy('label');
  $query->union($union);
  $results['factors'] = $query->execute()->fetchAllKeyed();

  // Country average.
  $query = db_select('ipvenger_request_detail', 'd');
  $query->addExpression('AVG(riskFactor)', 'average_score');
  $query->condition('d.country', $country);
  $results['average'] = $query->execute()->fetchField();

  // jVectorMap for the specific country.
  $library = libraries_load('ipvenger');
  if ($library['installed']) {
    $form['ipvenger-country-detail-wrapper']['country-detail-map'] = array(
      '#theme' => 'jvectormap',
      '#attributes' => array(
        'id' => 'ipvenger-country-details-map',
        'style' => 'width: 623px; height: 180px'),
    );
    $form['ipvenger-country-detail-wrapper']['country-detail-legend-wrapper'] = array(
      '#type' => 'container',
    );
    $form['ipvenger-country-detail-wrapper']['country-detail-legend-wrapper']['label'] = array(
      '#markup' => '<label>' . t('Average IPQ:') . '</label>',
    );
    $form['ipvenger-country-detail-wrapper']['country-detail-legend-wrapper']['low'] = array(
      '#markup' => '<div class="legend-color"><div class="legend-color-inner detail-legend-low"></div></div>' . '<label>' . '0 - 33' . '</label>',
    );
    $form['ipvenger-country-detail-wrapper']['country-detail-legend-wrapper']['medium'] = array(
      '#markup' => '<div class="legend-color"><div class="legend-color-inner detail-legend-medium"></div></div>' . '<label>' . '34 - 47' . '</label>',
    );
    $form['ipvenger-country-detail-wrapper']['country-detail-legend-wrapper']['high'] = array(
      '#markup' => '<div class="legend-color"><div class="legend-color-inner detail-legend-high"></div></div>' . '<label>' . '48 - 89' . '</label>',
    );
    $form['ipvenger-country-detail-wrapper']['country-detail-legend-wrapper']['critical'] = array(
      '#markup' => '<div class="legend-color"><div class="legend-color-inner detail-legend-critical"></div></div>' . '<label>' . '90 - 100' . '</label>',
    );

    $settings['blocked_countries'] = array();
    foreach ($results['blocked_countries'] as $result) {
      $settings['blocked_countries'][] = $result;
    }

    // Blocked country label details.
    foreach ($results['country_labels'] as $result) {
      $settings['country_labels'][] = $result;
    }
    $settings['selected_country'] = $country;
  }

  // Country disposition pie chart.
  $label_colors = array(
    'IPQ Score' => '#CC1A00',
    'Country Blacklist' => '#F6821F',
    'IP Blacklist' => '#999999',
    'Allowed' => '#00ADDE',
  );
  $datas = array();
  foreach ($label_colors as $label => $color) {
    $data = isset($results['allowed-blocked'][$label]) ? $results['allowed-blocked'][$label]->data : 0;
    $flot_data = new stdClass();
    $flot_data->data = (int) $data;
    $flot_data->label = $label;
    $flot_data->color = $color;
    switch ($label) {
      case 'IPQ Score':
        $flot_data->label = t('Failed IPQ Score');
        break;

      case 'Country Blacklist':
        $flot_data->label = t('Country blocked');
        break;

      case 'IP Blacklist':
        $flot_data->label = t('IP Blacklisted');
        break;

      case 'Allowed':
        $flot_data->label = t('Accepted Traffic');
        break;
    }
    $datas[] = $flot_data;
  }

  $disposition_options = new stdClass();
  $disposition_options->series->pie->show = TRUE;
  $disposition_options->series->pie->radius = 0.95;
  $disposition_options->series->pie->stroke->width = 3;
  $disposition_options->series->pie->label->show = FALSE;
  $disposition_options->grid->hoverable = TRUE;
  $disposition_options->legend->show = FALSE;
  $form['analytics-wrapper']['summary']['disposition']['graph'] = array(
    '#theme' => 'flot_graph',
    '#data' => $datas,
    '#element' => array(
      'id' => 'ipvenger-reports-disposition-pie',
      'class' => 'pie',
      'style' => 'width:130px;height:170px',
    ),
    '#options' => $disposition_options,
  );
  $text = t('@num_blocked IPs out of @totals requests were blocked from @start_date - !end_date', array(
    '@num_blocked' => $results['blocked_count'],
    '@totals' => $results['total_count'],
    '@start_date' => format_date($start_date, 'custom', 'F j, Y'),
    '!end_date' => format_date(REQUEST_TIME, 'custom', 'F j, Y'),
  ));
  $form['analytics-wrapper']['summary']['disposition']['top']['total_blocked'] = array(
    '#markup' => '<div id="ipvenger-reports-legend-title">' . $text . '</div>',
  );
  $text = t('Traffic from @country has an average IPQ score of @average_score.', array(
    '@country' => $country,
    '@average_score' => number_format($results['average'], 1),
  ));
  $form['analytics-wrapper']['summary']['disposition']['top']['average'] = array(
    '#markup' => '<div id="ipvenger-reports-legend-title">' . $text . '</div>',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper'] = array(
    '#type' => 'container',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-visits'] = array(
    '#markup' => '<div id="ipvenger-reports-legend-title">' . t('Reasons they were blocked') . '</div>',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-ipq'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('legend-ipq')),
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-ipq']['ipq-count'] = array(
    '#markup' => '<div class="disposition-legend-count">' . $datas[0]->data . '</div>',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-ipq']['ipq-label'] = array(
    '#markup' => '<div class="disposition-legend-label">' . t('Failed IPQ Score') . '</div>',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-country'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('legend-country')),
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-country']['country-count'] = array(
    '#markup' => '<div class="disposition-legend-count">' . $datas[1]->data . '</div>',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-country']['country-label'] = array(
    '#markup' => '<div class="disposition-legend-label">' . t('Country blocked') . '</div>',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-ip'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('legend-ip')),
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-ip']['ip-count'] = array(
    '#markup' => '<div class="disposition-legend-count">' . $datas[2]->data . '</div>',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-ip']['ip-label'] = array(
    '#markup' => '<div class="disposition-legend-label">' . t('IP Blacklisted') . '</div>',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-allowed'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('legend-allowed')),
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-allowed']['allowed-count'] = array(
    '#markup' => '<div class="disposition-legend-count">' . $datas[3]->data . '</div>',
  );
  $form['analytics-wrapper']['summary']['disposition']['disposition-wrapper']['disposition-graph-legend-wrapper']['legend-allowed']['allowed-label'] = array(
    '#markup' => '<div class="disposition-legend-label">' . t('Accepted Traffic') . '</div>',
  );

  // Render the category pie chart.
  // We need to make sure there are only four values to chart.
  $factors = $results['factors'];
  asort($factors);
  $factor_one = _ipvenger_reports_pop_key_value($factors);
  $factor_two = _ipvenger_reports_pop_key_value($factors);
  $factor_three = _ipvenger_reports_pop_key_value($factors);
  $factors_others['Other'] = array_sum($factors);
  $factors = array_merge($factor_one, $factor_two, $factor_three, $factors_others);

  $factor_colors = array(
    '#660000',
    '#CC0000',
    '#FF5500',
    '#FF9933',
  );

  $category_options = new stdClass();
  $category_options->series->pie->show = TRUE;
  $category_options->series->pie->radius = 0.95;
  $category_options->series->pie->stroke->width = 3;
  $category_options->series->pie->label->show = FALSE;
  $category_options->grid->hoverable = TRUE;
  $category_options->legend->show = FALSE;
  $category_options->legend->labelBoxBorderColor = '#FFF';
  $category_options->legend->labelFormatter = 'fn:Drupal.ipvenger_reports.pieLabelFormatter';
  $category_options->legend->container = '#edit-category-graph-legend';

  $datas = array();
  $iterator = 0;

  $rows = array();
  foreach ($factors as $label => $data) {
    $flot_data = new stdClass();
    $flot_data->data = (int) $data;
    $flot_data->label = $label;
    $flot_data->color = $factor_colors[$iterator];

    $datas[] = $flot_data;

    $rows[] = array(
      'data' => array(
        array(
          'data' => $data . '<div class="category-legend-count">' . $label . '</div>',
          'style' => array('color:' . $flot_data->color),
        ),
      ),
      'class' => array('legend-label'),
      'no_striping' => TRUE,
    );

    ++$iterator;
  }

  $form['analytics-wrapper']['summary']['category']['top']['graph'] = array(
    '#theme' => 'flot_graph',
    '#data' => $datas,
    '#element' => array(
      'id' => 'ipvenger-reports-category-pie',
      'class' => 'pie',
      'style' => 'width:130px;height:170px',
    ),
    '#options' => $category_options,
  );
  $form['analytics-wrapper']['summary']['category']['category-wrapper'] = array(
    '#type' => 'container',
  );
  $form['analytics-wrapper']['summary']['category']['category-wrapper']['top']['category-graph-legend-wrapper']['category-graph-legend-title'] = array(
    '#markup' => '<div id="country-category-pie-legend-title">Main threat categories:</div>',
  );
  $form['analytics-wrapper']['summary']['category']['category-wrapper']['top']['category-graph-legend-wrapper']['category-graph-legend'] = array(
    '#theme' => 'table',
    '#rows' => $rows,
    '#empty' => t('No results yet.'),
    '#attributes' => array('id' => 'category-pie-legend'),
  );

  $form['#attached']['js'] = array(
    drupal_get_path('module', 'ipvenger_reports') . '/js/ipvenger_reports.js' => array('type' => 'file'),
    drupal_get_path('module', 'ipvenger') . '/js/ipvenger.js' => array('type' => 'file'),
    drupal_get_path('module', 'ipvenger') . '/js/ipvenger.js' => array('type' => 'file'),
    libraries_get_path('ipvenger', TRUE) . '/js/ipvenger_country_codes.js' => array('type' => 'file'),
    libraries_get_path('ipvenger', TRUE) . '/js/ipvenger_world_en.js' => array('type' => 'file'),
    0 => array(
      'data' => array('ipvenger_reports' => $settings),
      'type' => 'setting'),
  );
  $form['#attached']['css'] = array(
    drupal_get_path('module', 'ipvenger_reports') . '/css/ipvenger_reports.css' => array('type' => 'file'),
    drupal_get_path('module', 'ipvenger') . '/css/ipvenger.css' => array('type' => 'file'),
  );

  return $form;
}

/**
 * Helper function to set the country filter appropriately.
 */
function _ipvenger_reports_country_filter($form, &$form_state) {
  return $form['analytics-wrapper']['country-table']['country-detail-table'];
}

/**
 * Helper function to pop an array slice.
 *
 * @param array $array
 *   Array (passed by reference) from which to pop.
 *
 * @return array
 *   The array slice popped from the end of the array.
 */
function _ipvenger_reports_pop_key_value(array &$array) {
  $slice = array();
  $value = end($array);
  if ($value) {
    $key = key($array);
    $slice = array($key => $value);
    unset($array[$key]);
  }
  return $slice;
}

/**
 * Submit handler for ipvenger_reports().
 *
 * @param array $form
 *   Form.
 *
 * @param array $form_state
 *   Form state.
 */
function ipvenger_reports_submit($form, $form_state) {
  drupal_goto(
    'admin/reports/ipvenger/analytics', array(
      'query' => array(
        'days' => $form_state['values']['date-range'],
      ),
    )
  );
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
function _ipvenger_reports_country_details_row_callback($row) {
  // We need a css class for styling.
  $country = $row['country'];
  $uri = 'admin/ipvenger/country-details/' . $country . '/' . $start_date;
  $uri = l($country, $uri, array('attributes' => array('target' => '_blank')));
  $row['country'] = array(
    'data' => $uri,
    'class' => 'ipvenger-country-name',
  );

  $row['blockedPercentage'] = number_format($row['blockedPercentage'], 2) . '%';
  $row['countryPercentage'] = number_format($row['countryPercentage'], 2) . '%';
  $row['averageScore'] = number_format($row['averageScore'], 2);

  // Change protection result into blacklist/protect.
  $protection = $row['protection'] == 'deny' ? t('Blacklisted') : t('Protected');
  unset($row['protection']);
  $row['protection'] = _ipvenger_reports_country_blacklist_select_list($protection, $country);

  return (array) $row;
}

/**
 * Helper function to render a select list for blacklist of a country.
 *
 * @param string $default_value
 *   The default value for the select list.
 * @param string $country
 *   The country for which to build the select list.
 *
 * @return string
 *   The html content to output.
 */
function _ipvenger_reports_country_blacklist_select_list($default_value, $country) {
  if (user_access('administer ipvenger')) {
    $render = array(
      '#type' => 'select',
      '#options' => array(
        'protect' => t('Protected'),
        'deny' => t('Blacklisted'),
      ),
      '#value' => $default_value,
      '#attributes' => array(
        'class' => array(
          str_replace(' ', '-', $country),
          'ipvenger',
        ),
      ),
    );

    return drupal_render($render);
  }
}
