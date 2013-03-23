/**
 * @file
 * The JavaScript behaviours for ipvenger_reports.
 */
(function ($) {
  /**
   * Register the ipvenger_reports behaviour.
   */
  Drupal.behaviors.ipvenger_reports = {
    attach: function (context, settings) {
      Drupal.ipvenger_reports.init(context, settings);
      Drupal.ipvenger_reports.recentBlocksTicker(context, settings);
      Drupal.ipvenger_reports.historyLookup(context, settings);
      Drupal.ipvenger_reports.countryDetailsMap(context, settings);
      Drupal.ipvenger_reports.countrySort(context, settings);
    }
  };

  /**
   * Class containing functionality for IPVenger Reports.
   */
  Drupal.ipvenger_reports = {};

  /**
   * Constructor for the Drupal.ipvenger_reports.init class.
   */
  Drupal.ipvenger_reports.init = function(context, settings) {
    $('#ipvenger-reports-disposition-pie', context).bind( 'plothover', Drupal.ipvenger_reports.plotHoverPercent );
    $('#ipvenger-reports-category-pie', context).bind( 'plothover', Drupal.ipvenger_reports.plotHoverPercent );
    $('#ipvenger-reports-daily-risk', context).bind( 'plothover', Drupal.ipvenger_reports.plotHoverBlocked );
    $('#ipvenger-reports-country-graph-percentage', context).bind( 'plothover', Drupal.ipvenger_reports.plotHoverCountry );
    $('#ipvenger-reports-country-graph-average-score', context).bind( 'plothover', Drupal.ipvenger_reports.plotHoverCountry );
  };
  
  /**
   * Constructor for the IPVenger Reports recentBlocksTicker class.
   */
  Drupal.ipvenger_reports.recentBlocksTicker = function(context, settings) {
    var recentBlocks = $('div#ipvenger-reports-recent-blocked', context);
    recentBlocks.load(settings.basePath + 'admin/ipvenger/recent-blocks' + ' div#block-system-main');
    setInterval(function() {
      recentBlocks.load(settings.basePath + 'admin/ipvenger/recent-blocks' + ' div#block-system-main');
    }, 10000);
  };

  /**
   * Constructor for the IPVenger Reports historyLookup class.
   */
  Drupal.ipvenger_reports.historyLookup = function(context, settings) {
    // Open dialog when selecting an IP address from the list.
    $('body').delegate('div.ipvenger-ip-address', 'click', function () {
      var ipAddress = $(this).html();
      Drupal.ipvenger.historyLookupDialog(context, settings, ipAddress);
    });
  };

  /**
   * Constructor for the IPVenger Reports showTooltip class.
   */
  Drupal.ipvenger_reports.showTooltip = function(x, y, contents) {
    $('<div id="ipvenger-reports-tooltip">' + contents + '</div>').css( {
      position: 'absolute',
      display: 'none',
      top: y + 5,
      left: x + 5,
      border: '1px solid #bcb',
      padding: '2px',
      'background-color': '#ede',
      opacity: 0.90,
    }).appendTo('body').fadeIn(200);
  };

  /**
   * Constructor for the IPVenger Reports plotHoverPercent class.
   */
  Drupal.ipvenger_reports.plotHoverPercent = function(event, pos, item) {
    var previousPoint = null;
    if (item) {
      if (previousPoint != item.series.angle) {
        previousPoint = item.series.angle;

        $('#ipvenger-reports-tooltip').remove();
        var y = parseFloat( item.series.percent ).toFixed(1);
        Drupal.ipvenger_reports.showTooltip(pos.pageX, pos.pageY-30, y + " %" );
      }
    }
    else {
        // workaround for flot pie bug that sometimes fails to
        // unhighlight a slice by creating a phony mousemove outside
        // the pie
        if ( pos.pageX != 1000 ) {
            var e = $.Event('mousemove');
            e.pageX = 1000;
            e.pageY = 1000;
            $( '#' + event.target.id + ' canvas:first').trigger(e);
        }

      $('#ipvenger-reports-tooltip').remove();
      previousPoint = null;
    }
  };

  /**
   * Constructor for the IPVenger Reports plotHoverCountry class.
   */
  Drupal.ipvenger_reports.plotHoverCountry = function(event, pos, item) {
    if (item) {
      if (previousPoint != item.dataIndex) {
        previousPoint = item.dataIndex;

        $('#ipvenger-reports-tooltip').remove();
        var y = (item.datapoint[1]-item.datapoint[2]).toFixed(0);
        Drupal.ipvenger_reports.showTooltip(pos.pageX, pos.pageY-30, y + " blocked" );

        y = item.datapoint[1].toFixed(1);
        if ( item.series.label == Drupal.t('Percent of blocked requests')) {
          Drupal.ipvenger_reports.showTooltip(pos.pageX, pos.pageY-30,y + "% of blocks" );
        }
        else {
          Drupal.ipvenger_reports.showTooltip(pos.pageX,pos.pageY-30, "Avg IPQ: " + y );
        }
      }
    }
    else {
      $('#ipvenger-reports-tooltip').remove();
      previousPoint = null;
    }
  };
  
  /**
   * Constructor for the IPVenger Reports plotHoverBlocked class.
   */
  Drupal.ipvenger_reports.plotHoverBlocked = function(event, pos, item) {
    var previousPoint = null;
    if (item) {
      if (previousPoint != item.datapoint) {
        previousPoint = item.datapoint;

        $('#ipvenger-reports-tooltip').remove();
        var y = (item.datapoint[1]-item.datapoint[2]).toFixed(0);
        Drupal.ipvenger_reports.showTooltip(pos.pageX, pos.pageY-30, y + ' ' + Drupal.t('blocked') );
      }
    }
    else {
      $('#ipvenger-reports-tooltip').remove();
      previousPoint = null;
    }
  };

  /**
   * Constructor for the IPVenger Reports countryTickFormatter class.
   */
  Drupal.ipvenger_reports.countryTickFormatter = function (val, axis) {
    // This is hackery, but the easiest way to avoid depending on orderBars.js
    // We pass in the countries values via a bogus country object.
    var countryLabel = '';
    for (var key in this.ipvenger_reports_country) {
      var country = this.ipvenger_reports_country[key];
      if (key == val) {
        countryLabel = country;
      }
    }
    return countryLabel;
  };

  /**
   * Constructor for the IPVenger Reports countryDetailsMap class.
   */
  Drupal.ipvenger_reports.countryDetailsMap = function (context, settings) {
    var mapObject = $('div#ipvenger-country-details-map', context);
    if (mapObject.length) {
      var hoverCountryColor = "#404040";
      
      mapObject.vectorMap({
        map: 'ipvenger_world_en',
        zoomOnScroll: false,
        zoomMax: 100,
        backgroundColor: "#F3F3F3",
        regionStyle: {
          initial: {
            'stroke-width': 0.25,
            stroke: "#F3F3F3",
            fill: "#A0A0A0",
          },
          hover: {
            fill: hoverCountryColor
          }
        },
        onRegionLabelShow: function (event, label, code) {

          name = label.text();
          var found = false;
          var country_labels = settings.ipvenger_reports.country_labels;
          for (var i = 0; i < country_labels.length; i++) {
            if (country_labels[i].label == name) {
              found = true;
              label.html("<b>" + name +
                "</b><br>&nbsp;&nbsp;% of Blocks: " +
                parseFloat(country_labels[i].percentage).toFixed(1) +
                "<br>&nbsp;&nbsp;Avg IPQ: " +
                parseFloat(country_labels[i].average_score).toFixed(1));
              break;
            }
          }
          if (!found) {
            label.html("<b>" + name + "<b><br>(no traffic)");
          }
        }
      });
      var map = mapObject.vectorMap('get', 'mapObject');

      $(settings.ipvenger_reports.country_labels).each(function () {
        var countryCode = ipvengerCountryCodeByCountry(this.label);
        if (typeof countryCode !== "undefined") {
          var color = '#5C0700';
          if (this.average_score < 34 ) {
            color = '#00A6E4';
          }
          else if ( this.average_score < 48 ) {
            color = '#EA7C1E';
          }
          else if ( this.average_score < 90 ) {
            color = '#B81600';
          }
          map.regions[countryCode].element.setStyle('fill', color);
        }
      });
      var selectedCountryCode = ipvengerCountryCodeByCountry(settings.ipvenger_reports.selected_country);
      map.setFocus(selectedCountryCode);
      map.regions[selectedCountryCode].element.setStyle('stroke', '#000' );
    }
  };

  /**
   * Constructor for the IPVenger Reports countrySort class.
   */
  Drupal.ipvenger_reports.countrySort = function (context, settings) {
    $('span#ipvenger-country-sort-block', context).click(function () {
      $("#ipvenger-country-sort-ipq").css("font-weight", "normal");
      $("#ipvenger-country-sort-block").css("font-weight", "bold");
      $("#edit-country-graph-container-percentage").show();
      $("#edit-country-graph-container-average-score").hide();
    });
    $('span#ipvenger-country-sort-ipq', context).click(function () {
      $("#ipvenger-country-sort-ipq").css("font-weight", "bold");
      $("#ipvenger-country-sort-block").css("font-weight", "normal");
      $("#edit-country-graph-container-percentage").hide();
      $("#edit-country-graph-container-average-score").show();
    });

  };

})(jQuery);
