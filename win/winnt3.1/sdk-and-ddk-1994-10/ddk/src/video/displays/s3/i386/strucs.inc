;-----------------------------------------------------------------------;
; POINTB is used for points with a range of -128..+127.
;-----------------------------------------------------------------------;

POINTB          STRUC
ptb_x           DB      0
ptb_y           DB      0
POINTB          ENDS

;-----------------------------------------------------------------------;
; POINTL is used for points with a range of +/- 2G.
;-----------------------------------------------------------------------;

POINTL          STRUC
ptl_x           DD      0
ptl_y           DD      0
POINTL          ENDS


;-----------------------------------------------------------------------;
; SIZEB is used for extents with a range of -128..+127.
;-----------------------------------------------------------------------;

SIZEB           STRUC
sizb_cx         DB      0
sizb_cy         DB      0
SIZEB           ENDS


;-----------------------------------------------------------------------;
; SIZEL is used for extents with a range of +/- 2G
;-----------------------------------------------------------------------;

SIZEL           STRUC
sizl_cx         DD      0
sizl_cy         DD      0
SIZEL           ENDS

;-----------------------------------------------------------------------;
; Definition of a rectangle
;-----------------------------------------------------------------------;


RECTL               struc           ; /* rcl */
xLeft               dd      ?
yTop                dd      ?
xRight              dd      ?
yBottom             dd      ?
RECTL               ends


;-----------------------------------------------------------------------;
; Definition of the physical device data structure
;
; NOTE: Changes to this structure must be reflected in driver.h!
;-----------------------------------------------------------------------;


PDEV                struc           ; /* pdev */
pdev_cur_x              dw  ?
pdev_cur_y              dw  ?
pdev_dest_x             dw  ?
pdev_dest_y             dw  ?
pdev_axstp              dw  ?
pdev_diastp             dw  ?
pdev_rect_width         dw  ?
pdev_line_max           dw  ?
pdev_err_term           dw  ?
pdev_gp_stat            dw  ?
pdev_cmd                dw  ?
pdev_short_stroke_reg   dw  ?
pdev_multifunc_cntl     dw  ?
pdev_bkgd_color         dw  ?
pdev_frgd_color         dw  ?
pdev_bkgd_mix           dw  ?
pdev_frgd_mix           dw  ?
pdev_wrt_mask           dw  ?
pdev_rd_mask            dw  ?
pdev_pixel_transfer     dw  ?
PDEV                ends

