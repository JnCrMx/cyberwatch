#include "cyberpunk_ui.hpp"
#include <algorithm>

static void draw_cyberpunk_border(lv_layer_t* layer, lv_draw_line_dsc_t& dsc, const lv_area_t& area, int offset) {
    dsc.p1.x = area.x1;
    dsc.p1.y = area.y1;
    dsc.p2.x = area.x2-offset/5;
    dsc.p2.y = area.y1;
    lv_draw_line(layer, &dsc);

    dsc.p1.x = area.x2-offset/5;
    dsc.p1.y = area.y1;
    dsc.p2.x = area.x2;
    dsc.p2.y = area.y1+offset/3;
    lv_draw_line(layer, &dsc);

    dsc.p1.x = area.x2;
    dsc.p1.y = area.y1+offset/3;
    dsc.p2.x = area.x2;
    dsc.p2.y = area.y2;
    lv_draw_line(layer, &dsc);

    dsc.p1.x = area.x2;
    dsc.p1.y = area.y2;
    dsc.p2.x = area.x1;
    dsc.p2.y = area.y2;
    lv_draw_line(layer, &dsc);

    dsc.p1.x = area.x1;
    dsc.p1.y = area.y2;
    dsc.p2.x = area.x1;
    dsc.p2.y = area.y1;
    lv_draw_line(layer, &dsc);
}

void cyberpunk_decoration_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target_obj(e);
    if(obj == nullptr) {
        return; // not a valid object
    }
    lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
    if(draw_task == nullptr) {
        return; // not a draw task event
    }

    lv_draw_dsc_base_t * base_dsc = (lv_draw_dsc_base_t *) lv_draw_task_get_draw_dsc(draw_task);
    if(base_dsc == nullptr) {
        return; // no draw descriptor available
    }

    lv_area_t a;
    lv_obj_get_coords(obj, &a);

    int w = a.x2 - a.x1;
    int h = a.y2 - a.y1;
    int offset = std::min(w, h);

    if(base_dsc->part == LV_PART_MAIN) {
        lv_draw_line_dsc_t line_dsc;
        lv_draw_line_dsc_init(&line_dsc);
        line_dsc.color = lv_obj_get_style_border_color(obj, LV_PART_MAIN);
        line_dsc.width = 2;
        draw_cyberpunk_border(base_dsc->layer, line_dsc, a, offset);
    } else if(lv_obj_check_type(obj, &lv_table_class) || lv_obj_check_type(obj, &lv_textarea_class)) {
        lv_draw_triangle_dsc_t tri_dsc;
        lv_draw_triangle_dsc_init(&tri_dsc);
        tri_dsc.bg_color = colors::BACKGROUND;
        tri_dsc.bg_opa = LV_OPA_COVER;
        tri_dsc.p[0].x = a.x2-offset/5;
        tri_dsc.p[0].y = a.y1-2;
        tri_dsc.p[1].x = a.x2+2;
        tri_dsc.p[1].y = a.y1-2;
        tri_dsc.p[2].x = a.x2+2;
        tri_dsc.p[2].y = a.y1+offset/3;
        lv_draw_triangle(base_dsc->layer, &tri_dsc);

        lv_draw_line_dsc_t line_dsc;
        lv_draw_line_dsc_init(&line_dsc);
        line_dsc.color = lv_obj_get_style_border_color(obj, LV_PART_MAIN);
        line_dsc.width = 2;
        draw_cyberpunk_border(base_dsc->layer, line_dsc, a, offset);
    }
}

void cyberpunk_decoration_apply(lv_obj_t* obj) {
    lv_obj_add_event_cb(obj, cyberpunk_decoration_cb, LV_EVENT_DRAW_TASK_ADDED, nullptr);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    lv_obj_set_style_border_opa(obj, LV_OPA_TRANSP, LV_PART_MAIN);
}

lv_style_t cyberpunk_style;
lv_theme_t cyberpunk_theme;

void init_cyberpunk_theme()
{
    lv_style_init(&cyberpunk_style);
    lv_style_set_bg_color(&cyberpunk_style, colors::BACKGROUND);
    lv_style_set_text_color(&cyberpunk_style, colors::PRIMARY);

    lv_theme_t * th_act = lv_display_get_theme(nullptr);
    cyberpunk_theme = *th_act;
    static auto theme_callback = [](lv_theme_t* th, lv_obj_t* obj){
        lv_obj_add_style(obj, &cyberpunk_style, 0);
    };
    lv_theme_set_parent(&cyberpunk_theme, th_act);
    lv_theme_set_apply_cb(&cyberpunk_theme, theme_callback);
}
