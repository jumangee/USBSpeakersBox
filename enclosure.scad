// sound blaster box, part1

//color("white")

panel_width = 250;
panel_height = 79;
panel_thickness = 5;
panel_hole_thickness = panel_thickness * 2+1;
panel_front_deep = 5;

border_thicknes = 7;
box_deep = 50;
box_width = panel_width + border_thicknes * 2;
box_height = panel_height + border_thicknes * 2;

speaker_hole_radius = 39;
speaker_screw_radius = 1;

border_round_offset = border_thicknes*2;

$fn=100;

module screwHoles(posx, posy, diff, offs) {
    translate([posx + diff+offs,posy + diff+offs,panel_front_deep*.9])
        cylinder(r=speaker_screw_radius,h=panel_hole_thickness);
    
    translate([posx - diff-offs,posy + diff+offs,panel_front_deep*.9])
        cylinder(r=speaker_screw_radius,h=panel_hole_thickness);
    
    translate([posx - diff-offs,posy - diff-offs,panel_front_deep*.9])
        cylinder(r=speaker_screw_radius,h=panel_hole_thickness);
    
    translate([posx + diff+offs,posy - diff-offs,panel_front_deep*.9])
        cylinder(r=speaker_screw_radius,h=panel_hole_thickness);
}

module speakerHole(posx, posy) {
    translate([posx,posy,panel_front_deep*.9])
        cylinder(r=speaker_hole_radius,h=panel_hole_thickness);
    
    screw_diff = speaker_hole_radius*sqrt(2)/2;
    
    //screwHoles(posx, posy, screw_diff, 0);
    //screwHoles(posx, posy, screw_diff, 1);
    screwHoles(posx, posy, screw_diff, 2);
    screwHoles(posx, posy, screw_diff, 2.5);
    screwHoles(posx, posy, screw_diff, 3);
    screwHoles(posx, posy, screw_diff, 3.5);
    screwHoles(posx, posy, screw_diff, 4);
    screwHoles(posx, posy, screw_diff, 4.5);
    screwHoles(posx, posy, screw_diff, 5);
    
    /*translate([posx + screw_diff,posy + screw_diff,panel_front_deep])
        cylinder(r=speaker_screw_radius,h=panel_hole_thickness);
    
    translate([posx - screw_diff,posy + screw_diff,panel_front_deep])
        cylinder(r=speaker_screw_radius,h=panel_hole_thickness);
    
    translate([posx - screw_diff,posy - screw_diff,panel_front_deep])
        cylinder(r=speaker_screw_radius,h=panel_hole_thickness);
    
    translate([posx + screw_diff,posy - screw_diff,panel_front_deep])
        cylinder(r=speaker_screw_radius,h=panel_hole_thickness);    */
}

difference() {
    difference() {
        difference() {
            minkowski()
            {
              cube([panel_width,panel_height,box_deep]);
              cylinder(r=border_thicknes,h=box_deep);
            }
            
            translate([border_thicknes,border_thicknes,-border_thicknes/2])
                minkowski()
                {
                  cube([panel_width-border_thicknes*2,panel_height-border_thicknes*2,panel_front_deep]);
                  cylinder(r=border_thicknes,h=panel_front_deep);
                }
        }
        
        translate([0,0,panel_front_deep+panel_thickness*2])
            cube([panel_width,panel_height,box_deep*1.85]);
    }
    
    // display hole
    translate([panel_width/2-15,panel_height/2,panel_front_deep*.9])
            cube([30,12, panel_hole_thickness]);
    
    // button 1
    translate([panel_width/2-15,panel_height/2+ 30,panel_front_deep*.9])
            cylinder(r=5,h=panel_hole_thickness);

    // button 2
    translate([panel_width/2+15,panel_height/2+ 30,panel_front_deep*.9])
            cylinder(r=5,h=panel_hole_thickness);
    
    // speaker hole 1
    speakerHole(panel_width/2-(panel_width/3)+1, panel_height/2);
    
    // speaker hole 2
    speakerHole(panel_width/2+(panel_width/3)-1, panel_height/2);
    /*translate([panel_width/2-(panel_width/3.5),panel_height/2,panel_front_deep])
            cylinder(r=30,h=panel_hole_thickness);
    
    translate([panel_width/2+(panel_width/3.5),panel_height/2,panel_front_deep])
            cylinder(r=30,h=panel_hole_thickness);*/
    

    // slider hole
    translate([panel_width/2-40,panel_height/2-20,panel_front_deep*.9])
            cube([80,1, panel_hole_thickness]);
    
}

/*
translate([0 - panel_width/2 - border_round_offset,0,panel_height/2 + border_round_offset])
    rotate([0, -90, -90])
        translate([0,0,0])
          rotate_extrude(angle=-90)
            translate([border_thicknes*2,0])
              square([border_thicknes, box_deep], center = true);


translate([0 - panel_width/2 - border_round_offset,0,0- panel_height/2 + border_round_offset])
rotate([0, -90, 0])
    rotate([0, -90, -90])
        translate([0,0,0])
          rotate_extrude(angle=-90)
            translate([border_thicknes*2,0])
              square([border_thicknes, box_deep], center = true);*/