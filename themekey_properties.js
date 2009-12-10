if (Drupal.jsEnabled) {

  var ThemeKey = ThemeKey || {};

  ThemeKey.oldParentId;

  ThemeKey.adjustChildCounter = function (parentId, adjust) {
    var childCounter = $('#themekey-num-childs-' + parentId);
    childCounter.val(parseInt(childCounter.val()) + adjust);
    if (1 == adjust) {
      $('.themekey-property-theme', $('#themekey-properties-row-' + parentId)).css('display', 'none');
    }
    else if (childCounter.val() < 1) {
      $('.themekey-property-theme', $('#themekey-properties-row-' + parentId)).css('display', 'block');
    }
  };

  ThemeKey.disableChilds = function (parentId) {
    var childRows = $("option[selected][value='" + parentId + "']", $('.themekey-property-parent')).parents("tr[id^='themekey-properties-row-']");
    childRows.each(function () {
      var childRow = $(this);
      var enabledElement = $('.themekey-property-enabled', childRow);
      enabledElement.css('display', 'none');
      if (enabledElement.attr('checked')) {
        enabledElement.removeAttr('checked');
        childRow.addClass('themekey-fade-out');
        ThemeKey.adjustChildCounter(parentId, -1);
        ThemeKey.disableChilds($('.themekey-property-id', childRow).val());
      }
    });
  };

  ThemeKey.allowEnablingDirectChilds = function (parentId) {
    var childRows = $("option[selected][value='" + parentId + "']", $('.themekey-property-parent')).parents("tr[id^='themekey-properties-row-']");
    childRows.each(function () {
      $('.themekey-property-enabled', $(this)).css('display', 'block');
    });
  };

  Drupal.tableDrag.prototype.onDrag = function() {
    ThemeKey.oldParentId = $('.themekey-property-parent', $(this.rowObject.element)).val();
    return null;
  };

  Drupal.tableDrag.prototype.onDrop = function() {
    if (this.changed) {
      var rowElement = $(this.rowObject.element);
      var newParentId = $('.themekey-property-parent', rowElement).val();
      var enabledElement = $('.themekey-property-enabled', rowElement);
      if (enabledElement.attr('checked')) {
        ThemeKey.adjustChildCounter(ThemeKey.oldParentId, -1);
        if (0 < newParentId) {
          var parentEnabledElement = $('.themekey-property-enabled', $('#themekey-properties-row-' + newParentId));
          if (parentEnabledElement.attr('checked')) {
            ThemeKey.adjustChildCounter(newParentId, 1);
          }
          else {
            enabledElement.removeAttr('checked');
            enabledElement.css('display', 'none');
            rowElement.addClass('themekey-fade-out');
            // hide and disable childs
            var id = enabledElement.attr('name').replace('old_items[', '').replace('][enabled]', '');
            ThemeKey.disableChilds(id);
          }
        }
        else {
         // no parent
         enabledElement.css('display', 'block');
        }
      }
      else {
        if (0 < newParentId) {
          var parentEnabledElement = $('.themekey-property-enabled', $('#themekey-properties-row-' + newParentId));
          if (parentEnabledElement.attr('checked')) {
            enabledElement.css('display', 'block');
          }
          else {
            enabledElement.css('display', 'none');
          }
        }
        else {
          // no parent
          enabledElement.css('display', 'block');
        }
      }
    }
    return null;
  };

  Drupal.behaviors.ThemeKey = function(context) {
    $('.themekey-property-property').change(
      function() {
        var wildcardElement = $('#' + $(this).attr('id').replace('property', 'wildcard'));
        if ('drupal:path:wildcard' == $(this).val()) {
          wildcardElement.css('display', 'block');
        }
        else {
          wildcardElement.css('display', 'none');
        }
      }
    );

    $('.themekey-property-enabled').change(
      function() {
        var id = $(this).attr('name').replace('old_items[', '').replace('][enabled]', '');
        var rowElement = $('#themekey-properties-row-' + id);
        var parentId = $('.themekey-property-parent', rowElement).val();
        if ($(this).attr('checked')) {
          rowElement.removeClass('themekey-fade-out');
          ThemeKey.adjustChildCounter(parentId, 1);
          ThemeKey.allowEnablingDirectChilds(id);
        }
        else {
          rowElement.addClass('themekey-fade-out');
          ThemeKey.adjustChildCounter(parentId, -1);
          // hide and disable childs
          ThemeKey.disableChilds(id);
        }
      }
    );
  };
}