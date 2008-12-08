<?php
// $Id: CoderTestCase.php,v 1.1.2.1 2008/01/19 19:21:16 sun Exp $

require_once drupal_get_path('module', 'coder') .'/scripts/coder_format/coder_format.inc';

class CoderTestCase extends DrupalTestCase {
  function assertFormat($input, $expect) {
    $result = coder_format_string_all($input);
    $this->assertIdentical($result, $input);
  }
}

