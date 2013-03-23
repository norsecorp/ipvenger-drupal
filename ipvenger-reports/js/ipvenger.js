/**
 * @file
 * The JavaScript behaviours for ipvenger.
 */
(function ($) {
  /**
   * Register the ipvenger behaviour.
   */
  Drupal.behaviors.ipvenger = {
    attach: function (context, settings) {
      Drupal.ipvenger.historyLookup(context, settings);
      Drupal.ipvenger.maskExceptionSelect(context, settings);
      Drupal.ipvenger.ipValidate(context, settings);
      Drupal.ipvenger.datepicker(context, settings);
      Drupal.ipvenger.blacklistCountryMap(context, settings);
    }
  };

  /**
   * Class containing functionality for IPVenger.
   */
  Drupal.ipvenger = {};

  /**
   * Constructor for the IPVenger historyLookup class.
   */
  Drupal.ipvenger.historyLookup = function (context, settings) {
    // Open dialog when looking up a specific IP addres.
    $('#edit-ip-lookup-submit', context).click(function () {
      var ipAddress = $('#edit-ip-lookup-textfield', context).val();
      Drupal.ipvenger.historyLookupDialog(context, settings, ipAddress);
    });
    // Open dialog when selecting an IP address from the table.
    $('td.ipvenger-ip-address', context).click(function () {
      var ipAddress = $(this, context).html();
      Drupal.ipvenger.historyLookupDialog(context, settings, ipAddress);
    });
  };

  /**
   * Constructor for the IPVenger historyLookupDialog class.
   */
  Drupal.ipvenger.historyLookupDialog = function (context, settings, ipAddress) {
    $('div.ipvenger-dialog').load(settings.basePath + 'admin/ipvenger/ip-history-lookup/' + ipAddress + ' #block-system-main')
      .dialog({
      modal: true,
      title: "IP Lookup - " + ipAddress,
      closeText: "Close",
      minWidth: 600,
      close: function (ev, ui) {
        $(this).dialog("destroy");
      }
    });
  };

  /**
   * Constructor for the IPVenger maskExceptionSelect class.
   */
  Drupal.ipvenger.maskExceptionSelect = function (context, settings) {
    $(context).delegate('select.ipvenger', 'change', function () {
      var maskClass = $(this, context).attr('class').split(/ +/)[0];
      // Are we dealing with an IP address or a country?
      var intRegex = /^\d$/;
      var mask = '';
      if(intRegex.test(maskClass.charAt(0))) {
        mask = maskClass.replace(new RegExp("-", "gm"), ".");
      }
      else {
        mask = maskClass.replace(new RegExp("-", "gm"), " ");
      }
      var newAction = $(this, context).val();
      var confirmed = false;
      if (newAction == 'deny') {
        confirmed = confirm(Drupal.t('Are you sure you want to block all traffic from @mask?', {
          '@mask': mask,
        }));
        Drupal.ipvenger.maskExceptionEntry(context, settings, confirmed, maskClass, 'deny', mask, this);
      } else if (newAction == 'allow') {
        confirmed = confirm(Drupal.t('Are you sure you want to allow all traffic from @mask regardless of IPQ Score?', {
          '@mask': mask,
        }));
        Drupal.ipvenger.maskExceptionEntry(context, settings, confirmed, maskClass, 'allow', mask, this);
      } else if (newAction == 'protect') {
        confirmed = confirm(Drupal.t('Are you sure you want to resume normal IPVenger processing for @mask?', {
          '@mask': mask,
        }));
        Drupal.ipvenger.maskExceptionEntry(context, settings, confirmed, maskClass, 'protect', mask, this);
      }
      $.data(this, 'val', newAction); // Store new value for next time.
    });
  };

  /**
   * Constructor for the IPVenger maskExceptionEntry class.
   */
  Drupal.ipvenger.maskExceptionEntry = function (context, settings, confirmed, maskClass, action, mask, selectedOption) {
    if (confirmed) {
      var url = settings.basePath + 'admin/ipvenger/mask-action/' + action + '/' + mask;
      $.get(url);
      $('select.' + maskClass, context).val(action);
    } else {
      $(selectedOption, context).val($.data(selectedOption, context, 'val')); // Reset to previous value.
    }
  };

  /**
   * Constructor for the IPVenger ipValidate class.
   */
  Drupal.ipvenger.ipValidate = function (context, settings) {
    // Attach the realtime IP address validation handler.
    $('#edit-ip-lookup-textfield', context).bind('propertychange keyup input paste', function () {
      var pattern = /^(([0-9\*]|[1-9][0-9\*]|1[0-9][0-9\*]|2[0-4][0-9\*]|25[0-5\*])\.){3}([0-9\*]|[1-9][0-9\*]|1[0-9][0-9\*]|2[0-4][0-9\*]|25[0-5\*])$/g;
      if (pattern.test($(this, context).val())) $('input[type="submit"].ipvenger-ip-address', context).removeAttr('disabled').removeClass('form-button-disabled');
      else {
        $('input[type="submit"].ipvenger-ip-address', context).attr('disabled', 'disabled').addClass('form-button-disabled');
      }
    });
  };

  /**
   * Constructor for the IPVenger datepicker class.
   */
  Drupal.ipvenger.datepicker = function (context, settings) {
    if (typeof $.datepicker != 'undefined') {
      $('#edit-start-date', context).add('#edit-end-date', context).datepicker({
        dateFormat: 'yy-mm-dd',
        minDate: '-30',
        maxDate: '+0',
      });
    }
  };

  /**
   * Constructor for the IPVenger blacklistCountryMap class.
   */
  Drupal.ipvenger.blacklistCountryMap = function (context, settings) {
    var blacklistCountryColor = '#F00000';
    var safeCountryColor = '#A0A0A0';
    var hoverCountryColor = "#404040";

    var mapObject = $('div#ipvenger-country-map', context);
    if (mapObject.length) {
      mapObject.vectorMap({
        map: 'ipvenger_world_en',
        zoomMax: 100,
        backgroundColor: "#FFFFFF",
        regionStyle: {
          initial: {
            fill: safeCountryColor,
            "stroke-width": 0.5
          },
          hover: {
            fill: hoverCountryColor
          }
        },
        onRegionLabelShow: function (event, label, code) {

          name = label.text();
          var found = false;
          var country_labels = settings.ipvenger.country_labels;
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

      $(settings.ipvenger.blocked_countries).each(function () {
        var countryCode = ipvengerCountryCodeByCountry(this);
        if (typeof countryCode !== "undefined") {
          map.regions[countryCode].element.setStyle('fill', blacklistCountryColor);
        }
      });
      map.reset();
    }
  };

})(jQuery);
