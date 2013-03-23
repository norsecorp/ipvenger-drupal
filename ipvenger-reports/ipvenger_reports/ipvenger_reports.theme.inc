<?php
/**
 * @file
 * Implements IPVenger Reporting themes in Drupal.
 */

/**
 * Theme override for theme_flot_graph().
 */
function theme_ipvenger_reports_flot_graph($variables) {
  $element = isset($variables['element']) ? $variables['element'] : array();
  $datas = isset($variables['data']) ? $variables['data'] : array();
  $options = isset($variables['options']) ? $variables['options'] : new stdClass();

  static $n = 0;
  $zoom = (isset($variables['addselectionfilter']) && $variables['addselectionfilter']) || (isset($variables['zoom']) && $variables['zoom']);

  if ($zoom) {
    $element['class'] = (isset($element['class'])) ? $element['class'] . ' flot-with-zoom' : 'flot-with-zoom';
    $element['id'] = 'flot-view-zoomable-' . ++$n;
    $options->selection['mode'] = 'x';
  }
  else {
    if (!isset($element['id'])) {
      ++$n;
      $element['id'] = 'flot-auto-identifier-' . $n;
    }
  }

  if (!isset($element['style'])) {
    $element['style'] = "width:100%;height:200px";
  }

  // Setup some sane defaults for legend positioning.  If you don't specify a
  // container but want to display a legend, then let's assign an appropriate
  // CSS class to the item.
  $legend_element = array();
  if ((isset($options->legend->show) && $options->legend->show && empty($options->legend->container)) && (empty($options->legend->position) || $options->legend->position == 'bottom')) {
    $element['legendid'] = $element['id'] . '-legend';
    $options->legend->container = '#' . $element['legendid'];
    $legend_element = array(
      'id' => $element['legendid'],
      'class' => array('legend', 'legend-bottom'),
    );
  }

  flot_add_js('core');
  if (isset($options->selection)) {
    flot_add_js('selection');
  }
  if (isset($options->series->pie->show) && $options->series->pie->show) {
    flot_add_js('pie');
  }
  if (isset($options->series->stack) && $options->series->stack) {
    flot_add_js('stack');
  }

  $json_datas = drupal_json_encode((array) $datas);
  $json_options = drupal_json_encode($options);
  // Adding per https://drupal.org/node/1072046.
  $regex = '/"(labelFormatter|transform|formatter|inverseTransform|ticks|tickFormatter|symbol|markings)":"fn:([\w_\-\.]+)"/i';
  $replace = '"$1":$2';
  $json_options = preg_replace($regex, $replace, $json_options);

  $jsdata = '
    jQuery(document).ready(function() {
      Drupal.flot[\'' . str_replace('-', '_', $element['id']) . '\'] = {};
      Drupal.flot[\'' . str_replace('-', '_', $element['id']) . '\'][\'flot\'] = jQuery.plot(jQuery("#' . $element['id'] . '"), ' . $json_datas . ', ' . $json_options . ');
      Drupal.flot[\'' . str_replace('-', '_', $element['id']) . '\'][\'options\'] = ' . $json_options . ';
      Drupal.flot[\'' . str_replace('-', '_', $element['id']) . '\'][\'data\'] = ' . $json_datas . ';
    });
  ';
  drupal_add_js($jsdata, array('type' => 'inline', 'scope' => 'footer'));

  $element = array(
    'graph' => array(
      'style' => $element['style'],
      'class' => isset($element['class']) ? array($element['class']) : array(),
      'id' => $element['id'],
    ),
  );

  $zoom_element = array();
  if ($zoom && (!isset($options->series->pie->show) || $options->series->pie->show == FALSE)) {
    $options->series->points->show = FALSE;
    $options->series->lines->lineWidth = 1;
    $options->series->shadowSize = 0;
    $options->grid->hoverable = FALSE;
    $options->selection['mode'] = 'x';
    $options->legend->show = FALSE;
    if (isset($options->xaxis)) {
      unset($options->xaxis->min);
      unset($options->xaxis->max);
    }
    if (isset($options->yaxis)) {
      unset($options->yaxis->min);
      unset($options->yaxis->max);
      unset($options->yaxis->autoscaleMargin);
    }

    // Adding per https://drupal.org/node/1072046.
    $json_options = drupal_json_encode($options);
    $json_options = preg_replace($regex, $replace, $json_options);

    $jszoomdata = '
    jQuery(document).ready(function() {
      Drupal.flot[\'' . str_replace('-', '_', $element['id'] . '-zoom') . '\'] = {};
      Drupal.flot[\'' . str_replace('-', '_', $element['id'] . '-zoom') . '\'][\'flot\'] = jQuery.plot(jQuery("#' . $element['id'] . '"), ' . $json_datas . ', ' . $json_options . ');
      Drupal.flot[\'' . str_replace('-', '_', $element['id'] . '-zoom') . '\'][\'options\'] = ' . $json_options . ';
      Drupal.flot[\'' . str_replace('-', '_', $element['id'] . '-zoom') . '\'][\'data\'] = ' . $json_datas . ';
    });
  ';
    drupal_add_js($jszoomdata, array('type' => 'inline', 'scope' => 'footer'));
    $width = array();
    preg_match('/width:(.*)px;/', $element['graph']['style'], $width);
    $zoom_element = array(
      'id' => $element['graph']['id'] . '-zoom',
      'style' => 'width:' . $width[1] . 'px;height:100px',
      'class' => array('flot-zoom'),
    );
  }

  $output = array(
    'graph' => array(
      '#type' => 'container',
      '#attributes' => $element['graph'],
    ),
  );
  if (!empty($zoom_element)) {
    $output[] = array(
      'zoom' => array(
        '#type' => 'container',
        '#attributes' => $zoom_element,
      ),
    );
  }
  if (!empty($legend_element)) {
    $output[] = array(
      'legend' => array(
        '#type' => 'container',
        '#attributes' => $legend_element,
      ),
    );
  }
  return drupal_render($output);
}
