#include <pebble.h>

//144x168

static Window *my_window;
static TextLayer *text_layer;
static TextLayer *s_time_layer;
static Layer *s_lyrGrp;
static BitmapLayer *s_lyrBG;
static GBitmap *s_bmpBG;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  static uint32_t c = 0;
  static char buf[64];
  snprintf(buf, sizeof(buf), "TEST TEST SSS\nあいう\n%lu\nHeapFree:%d\nHeapUsed:%d",
           c++, heap_bytes_free(), heap_bytes_used()
          );
 
  text_layer_set_text(text_layer, buf);
  
  
  /** Display the Time **/
  time_t temp = time(NULL);
//  struct tm *tick_time = localtime(&temp);
  tick_time = localtime(&temp);

  static char s_time_buffer[8];
  strftime(s_time_buffer, sizeof(s_time_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  text_layer_set_text(s_time_layer, s_time_buffer);
  
}

static void layer_updateCB(Layer *me, GContext *ctx)
{
  graphics_context_set_fill_color(ctx, GColorClear);
  graphics_context_set_stroke_color( ctx, GColorFromRGBA(
    rand() % 256,
    rand() % 256,
    rand() % 256,
    128 + rand() % 128
  ) );
  graphics_draw_circle( ctx, GPoint( 100, 100 ), 5 + rand()%30 );
}

static void appTimerCB( void *data) 
{
    layer_mark_dirty( s_lyrGrp );
    //再設定必要
    app_timer_register( 16, appTimerCB, NULL );
}

void handle_init(void)
{
  //--- main window
  my_window = window_create();
  Layer* winlayer = window_get_root_layer( my_window );
  GRect rect = layer_get_bounds( winlayer );
  
  //#### 以下、最初に addchild したもの程下に描画される ###
  
  //** bmp
//  s_bmpBG = gbitmap_create_with_resource( RESOURCE_ID_IMAGE_BG64 );
  s_bmpBG = gbitmap_create_with_resource( RESOURCE_ID_IMAGE_SP671 );
  
  s_lyrBG = bitmap_layer_create( layer_get_bounds( winlayer ) );
  bitmap_layer_set_bitmap( s_lyrBG, s_bmpBG );
  layer_add_child( winlayer, bitmap_layer_get_layer( s_lyrBG ) );
  
  
  //*** msg layer
  text_layer = text_layer_create(rect);
  text_layer_set_text(text_layer, "TEST TEST SSS\nあいう");
  text_layer_set_background_color( text_layer, GColorClear );
  text_layer_set_text_color( text_layer, GColorMintGreen );
  //add child で表示される
  layer_add_child( winlayer, text_layer_get_layer(text_layer) ); 
  
  //*** Create time TextLayer
  GRect bounds = layer_get_bounds(winlayer);
//  s_time_layer = text_layer_create( rect );//GRect(0, bounds.size.h, bounds.size.w, 50) );
  s_time_layer = text_layer_create( GRect( 0, 50, 40, 40 )); //GRect(x, y, w, h)
  text_layer_set_background_color(s_time_layer, GColorBlack );//GColorClear);
  text_layer_set_text_color(s_time_layer, GColorOrange );// GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  layer_add_child(winlayer, text_layer_get_layer(s_time_layer));
  //位置  
  GRect time_frame = layer_get_frame(text_layer_get_layer(s_time_layer));
  time_frame.origin.y = 80;
  layer_set_frame(text_layer_get_layer(s_time_layer), time_frame);

  //*** grphic
  GRect frame = layer_get_frame(winlayer);
  s_lyrGrp = layer_create( frame );
  layer_add_child( winlayer, s_lyrGrp );
  
  
  // window セット
  window_stack_push(my_window, true);
  
  
  //--------------- 更新用関数
  //  layer_set_update_proc( Layer* layer, LayerUpdateProc update_proc ); <<<< 描画が必要な際に呼ばれるやつ
  layer_set_update_proc( s_lyrGrp, layer_updateCB );
  
  // Register with TickTimerService
  // AppTimer * app_timer_register(uint32_t timeout_ms, AppTimerCallback callback, void * callback_data) <<<< ms 単位の好きな時間でできる
  // tick_timer_service_subscribe( TimeUnits tick_units, TickHandler handler ); <<<< 秒、分、時間、日、月、年単位で呼ばれる
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  
  app_timer_register( 16, appTimerCB, NULL );
}

void handle_deinit(void) {
  text_layer_destroy(text_layer);
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
