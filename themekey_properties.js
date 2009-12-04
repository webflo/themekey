if (Drupal.jsEnabled) {

  var ThemeKey = ThemeKey || {};
  ThemeKey.oldParentId;
  ThemeKey.adjustChildCounter = function (parentId, adjust) {
    var childCounter = $('#themekey-num-childs-' + parentId);
    childCounter.val(parseInt(childCounter.val()) + adjust);
    if (1 == adjust) {
      $('.themekey-property-theme', $('#themekey-properties-row-' + parentId)).addClass('themekey-fade-out');
    }
    else if (childCounter.val() < 1) {
      $('.themekey-property-theme', $('#themekey-properties-row-' + parentId)).removeClass('themekey-fade-out');
    }
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
        var parentEnabledElement = $('.themekey-property-enabled', $('#themekey-properties-row-' + newParentId));
        if (parentEnabledElement.attr('checked')) {
          ThemeKey.adjustChildCounter(newParentId, 1);
        }
        else {
          enabledElement.addClass('themekey-property-enabled-parent-' + newParentId);
          enabledElement.removeAttr('checked');
          enabledElement.css('display', 'none');
          rowElement.addClass('themekey-fade-out');
        }
      }
      else {
        enabledElement.removeClass('themekey-property-enabled-parent-' + ThemeKey.oldParentId);
        var parentEnabledElement = $('.themekey-property-enabled', $('#themekey-properties-row-' + newParentId));
        if (parentEnabledElement.attr('checked')) {
          enabledElement.css('display', 'block');
        }
        else {
          enabledElement.addClass('themekey-property-enabled-parent-' + newParentId);
          enabledElement.css('display', 'none');
        }
      }
    }
    return null;
  };

  Drupal.behaviors.theme_key = function(context) {
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
          $('.themekey-property-enabled-parent-' + id).css('display', 'block');
        }
        else {
          rowElement.addClass('themekey-fade-out');
          ThemeKey.adjustChildCounter(parentId, -1);
          // TODO hide and disable childs
        }
      }
    );
  };
}
