<?script
    function on_draw($cr, $hello) {
    }
    $document->body->h1 = "Hello World";// build tree
    $document['h1/span'][0] = "Hello World";// search tree
?>
<my:widget draw:after="on_draw(..., $(../@user-data));" user-data="Hello" >
    <GtkWidgetClass:draw src="image.svg">
        <rect x="0" y="0" />
    </GtkWidgetClass:draw>
    <gtk:label>Child</gtk:label>
</my:widget>
---------------------------------------
<?php

class MyWidget extends GtkWidget {
    /** @override GtkWidgetClass.draw */
    protected function draw($cr):void {
        cairo_set_source_rgb($cr, 0, 0, 1);
        cairo_rectangle($cr, 0, 0, 10, 10);
        cairo_fill($cr);
    }
}

GTKML::Register('widget', "MyWidget");
GTKML::RegisterNs('http://.../my.dtd', 'widget', "MyWidget");
