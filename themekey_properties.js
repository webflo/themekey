if (Drupal.jsEnabled) {
  Drupal.behaviors.theme_key = function(context) {
    $('.themekey-property-property').change(
      function() {
        var wildcardElement = $('#' + $(this).attr('id').replace('property', 'wildcard'));
        if ("drupal:path:wildcard" == $(this).val()) {
          wildcardElement.css('display', 'block');
        }
        else {
          wildcardElement.css('display', 'none');
        }
      }
    );
  };
}	
