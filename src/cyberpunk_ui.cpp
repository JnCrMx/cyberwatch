#include "cyberpunk_ui.hpp"
#include <algorithm>

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

    if(base_dsc->part == LV_PART_MAIN) {
        lv_draw_line_dsc_t draw_dsc;
        lv_draw_line_dsc_init(&draw_dsc);
        draw_dsc.color = lv_obj_get_style_border_color(obj, LV_PART_MAIN);
        draw_dsc.width = 2;

        lv_area_t a;
        lv_obj_get_coords(obj, &a);

        int w = a.x2 - a.x1;
        int h = a.y2 - a.y1;
        int offset = std::min(w, h);

        draw_dsc.p1.x = a.x1;
        draw_dsc.p1.y = a.y1;
        draw_dsc.p2.x = a.x2-offset/5;
        draw_dsc.p2.y = a.y1;
        lv_draw_line(base_dsc->layer, &draw_dsc);

        draw_dsc.p1.x = a.x2-offset/5;
        draw_dsc.p1.y = a.y1;
        draw_dsc.p2.x = a.x2;
        draw_dsc.p2.y = a.y1+offset/3;
        lv_draw_line(base_dsc->layer, &draw_dsc);

        draw_dsc.p1.x = a.x2;
        draw_dsc.p1.y = a.y1+offset/3;
        draw_dsc.p2.x = a.x2;
        draw_dsc.p2.y = a.y2;
        lv_draw_line(base_dsc->layer, &draw_dsc);

        draw_dsc.p1.x = a.x2;
        draw_dsc.p1.y = a.y2;
        draw_dsc.p2.x = a.x1;
        draw_dsc.p2.y = a.y2;
        lv_draw_line(base_dsc->layer, &draw_dsc);

        draw_dsc.p1.x = a.x1;
        draw_dsc.p1.y = a.y2;
        draw_dsc.p2.x = a.x1;
        draw_dsc.p2.y = a.y1;
        lv_draw_line(base_dsc->layer, &draw_dsc);
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
