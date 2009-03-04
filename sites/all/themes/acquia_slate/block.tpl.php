<?php
// $Id: block.tpl.php,v 1.1 2009/02/28 23:33:58 jwolf Exp $
?>
<!-- start block.tpl.php -->
<div class="block-wrapper <?php print $block_zebra; ?>">

  <?php if ($is_front AND $rounded_block): ?>
  <div class="rounded-block">
    <div class="rounded-block-top-left"></div>
    <div class="rounded-block-top-right"></div>
    <div class="rounded-outside">
      <div class="rounded-inside">
        <p class="rounded-topspace"></p>
  <?php endif; ?>
      
        <div id="block-<?php print $block->module .'-'. $block->delta; ?>" class="block block-<?php print $block->module ?>">
          <?php if ($block->subject): ?>
          <?php $firstword = wordlimit($block->subject, 1, "");
          $block->subject = str_replace($firstword, "<span class=\"first-word\">" . $firstword . "</span>", $block->subject); ?>
          <h2 class="title block-title"><?php print $block->subject ?></h2>
          <?php endif;?>
          <div class="content">
            <?php print $block->content ?>
          </div>
        </div>
  
  <?php if ($is_front AND $rounded_block): ?>
        <p class="rounded-bottomspace"></p>
      </div><!-- /rounded-inside -->
    </div>
    <div class="rounded-block-bottom-left"></div>
    <div class="rounded-block-bottom-right"></div>
  </div><!-- /rounded-block -->
  <?php endif; ?>
  
</div>
<!-- /end block.tpl.php -->