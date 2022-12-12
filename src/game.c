#include "game.h"

/// @brief Draw the play field
void draw_field() {
    // top edge
    _ssd1306_line(&display, 0, 0, 127, 0, true);
    // bottom edge
	_ssd1306_line(&display, 0, 63, 127, 63, false);
    // display edges
	ssd1306_show_buffer(&display);
}


/// @brief Draw the stick
/// @param stick stick structure
void draw_stick(Stick stick) {
    // set first column
    _ssd1306_line(&display, stick.x_pos, stick.y_pos, stick.x_pos, stick.y_pos+stick.length, false);
    // set second column
	_ssd1306_line(&display, stick.x_pos+1, stick.y_pos, stick.x_pos+1, stick.y_pos+stick.length, false);
    // draw stick
    ssd1306_show_buffer(&display);
}


/// @brief Update stick
/// @param stick stick structure
void update_stick(Stick stick) {
    // erase old position pixels if sticke moved 
    if(stick.y_speed < 0) {
        _ssd1306_line(&display, stick.x_pos, stick.y_pos + stick.length - stick.y_speed, stick.x_pos, stick.y_pos + stick.length + 1, true);
        _ssd1306_line(&display, stick.x_pos + 1, stick.y_pos + stick.length - stick.y_speed, stick.x_pos + 1,stick.y_pos + stick.length + 1, true);
    } else if(stick.y_speed > 0) {
        _ssd1306_line(&display, stick.x_pos, stick.y_pos - stick.y_speed, stick.x_pos, stick.y_pos - 1, true);
        _ssd1306_line(&display, stick.x_pos + 1, stick.y_pos - stick.y_speed, stick.x_pos + 1, stick.y_pos - 1, true);
    }
    ssd1306_show_buffer(&display);

    // draw dtick at new position
    if(stick.y_speed != 0) {
        draw_stick(stick);
    }
}


/// @brief Draw a ball
/// @param ball ball structure
void draw_ball(Ball ball) {
    // set ball pixels
    _ssd1306_pixel(&display, ball.x_pos, ball.y_pos + 1, false);
    _ssd1306_pixel(&display, ball.x_pos + 1, ball.y_pos, false);
    _ssd1306_pixel(&display, ball.x_pos, ball.y_pos, false);
    _ssd1306_pixel(&display, ball.x_pos - 1, ball.y_pos, false);
    _ssd1306_pixel(&display, ball.x_pos, ball.y_pos - 1, false);
    // draw ball
    ssd1306_show_buffer(&display);
}


/// @brief Update ball 
/// @param ball ball structure
void update_ball(Ball ball) {
    // erase old position pixels of the ball
    _ssd1306_pixel(&display, ball.x_pos_old, ball.y_pos_old, true);
    _ssd1306_pixel(&display, ball.x_pos_old + 1, ball.y_pos_old, true);
    _ssd1306_pixel(&display, ball.x_pos_old - 1, ball.y_pos_old, true);
    _ssd1306_pixel(&display, ball.x_pos_old, ball.y_pos_old + 1, true);
    _ssd1306_pixel(&display, ball.x_pos_old, ball.y_pos_old - 1, true);
    ssd1306_show_buffer(&display);
    // draw ball
    draw_ball(ball);
}


/// @brief initialize display
void init_display() {
    i2c_master_init(&display, 
                    CONFIG_SDA_GPIO,
                    CONFIG_SCL_GPIO,
                    CONFIG_RESET_GPIO);
    ssd1306_init(&display, 128, 64);
    
}


/// @brief draw all game objects
void draw_objects() {
    draw_field();
    draw_stick(pong.player);
    draw_stick(pong.bot);
    draw_ball(pong.ball);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

/// @brief Function to handle joystick status
/// @param pvParameter task parameter(NULL)
void joystick_task(void *pvParameter) {
    while(1) {
        // get raw value 
        int y_axis_val = adc1_get_raw(JOYSTICK_Y_AXIS);
        // set player speed according to joystick Y axis value
        if(y_axis_val >= 0 && y_axis_val <= 500) {
            pong.player.y_speed = 6;
        } else if (y_axis_val > 500 && y_axis_val <= 1000) {
            pong.player.y_speed = 3;
        } else if (y_axis_val > 1000 && y_axis_val <= 1800) {
            pong.player.y_speed = 1;
        } else if (y_axis_val > 1800 && y_axis_val <= 2200) {
            pong.player.y_speed = 0;
        } else if (y_axis_val > 2200 && y_axis_val <= 3000) {
            pong.player.y_speed = -1;
        } else if (y_axis_val > 3000 && y_axis_val <= 3500) {
            pong.player.y_speed = -3;
        } else {
            pong.player.y_speed = -6;
        }
        // delay to make space for other tasks to run
		vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}


/// @brief initialize joystick
void init_joystick() {
    // set width (0-4095)
    adc1_config_width(ADC_WIDTH_BIT_12);
    // configure the channel
    adc1_config_channel_atten(JOYSTICK_Y_AXIS, ADC_ATTEN_DB_11);
}

/// @brief initialize game objects 
void init_game_objects() {
    pong.field.x0 = 1;
    pong.field.x1 = 126;
    pong.field.y0 = 1;
    pong.field.y1 = 62;
    pong.field.width = 126;
    pong.field.height = 62;

    pong.ball.x_pos = 64;
    pong.ball.y_pos = 32;
    pong.ball.x_speed = 5;
    pong.ball.y_speed = 5;
    pong.ball.radius = 1;

    pong.player.length = 12;
    pong.player.x_pos = 0;
    pong.player.y_pos = pong.field.y0;
    pong.player.y_speed = 0;

    pong.bot.length = 12;
    pong.bot.x_pos = 126;
    pong.bot.y_pos = 32;
    pong.bot.y_speed = 5;

    pong.score1 = 0;
    pong.score2 = 0;
}


/// @brief Process score
void process_score() {

    if(pong.score1 == 5) {
        // clear the screen
        ssd1306_clear_screen(&display, false);
        // print text
        ssd1306_display_text(&display, 3, "!!!PLAYER WON!!!", 16, false);   
        // wait
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // clear the screen
        ssd1306_clear_screen(&display, false);
        // reset score
        pong.score1 = 0;
        pong.score2 = 0;
    } else if (pong.score2 == 5) {
        // clear the screen
        ssd1306_clear_screen(&display, false);
        // print text
        ssd1306_display_text(&display, 3, "!!!AI BOT WON!!!", 16, false);
        // wait
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // clear the screen
        ssd1306_clear_screen(&display, false);
        // reset score
        pong.score1 = 0;
        pong.score2 = 0;
    } else {
        // make string with score
        char score[5];
        snprintf(score, 5, " %d:%d", pong.score1, pong.score2);
        // clear the screen
        ssd1306_clear_screen(&display, false);
        // print score
        ssd1306_display_text_x3(&display, 3, score, strlen(score), false);
        // wait
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // clear the screen
        ssd1306_clear_screen(&display, false);

    }
    // set ball position to the center of the screen
    pong.ball.x_pos = 64;
    pong.ball.y_pos = 32;
    // redraw game objects
    draw_objects();
}


/// @brief process ball
void process_ball() {
    // save old position
    pong.ball.x_pos_old = pong.ball.x_pos;
    pong.ball.y_pos_old = pong.ball.y_pos;

    // check left border
    if((pong.ball.x_pos - pong.ball.radius + pong.ball.x_speed) <= 2)
    {   
        
        if(((pong.ball.y_pos - pong.ball.radius) <= pong.player.y_pos + pong.player.length) &&
           ((pong.ball.y_pos + pong.ball.radius) >= pong.player.y_pos))
        {
            // ball hits stick
            if(pong.ball.y_pos < pong.player.y_pos + 6) {
                pong.ball.y_speed = -5;
            } else {
                pong.ball.y_speed = 5;
            }
            pong.ball.x_pos = 3;
            pong.ball.x_speed*=-1;
        } else {
            // ball hits left border
            pong.score2++;
            process_score();
            return;
        }
        
    } else if((pong.ball.x_pos - pong.ball.radius + pong.ball.x_speed) >= 125) {
        // check right border
        if(((pong.ball.y_pos - pong.ball.radius) <= pong.bot.y_pos + pong.bot.length) &&
           ((pong.ball.y_pos + pong.ball.radius) >= pong.bot.y_pos))
        {
            // ball hits stick
            if(pong.ball.y_pos < pong.bot.y_pos + 6) {
                pong.ball.y_speed = -5;
            } else {
                pong.ball.y_speed = 5;
            }
            pong.ball.x_pos = 124;
            pong.ball.x_speed*=-1;
        } else {
            // ball hits right border
            pong.score1++;
            process_score();
            return;
        }
    } else {
        // ball between borders
        pong.ball.x_pos+=pong.ball.x_speed;
    }
    // check y axis ball position
    if(((pong.ball.y_pos + pong.ball.radius + pong.ball.y_speed) >= pong.field.y1) ||
        ((pong.ball.y_pos - pong.ball.radius + pong.ball.y_speed) <= pong.field.y0)) {
        // ball hits top/bottom border
        if(pong.ball.y_speed > 0) {
            pong.ball.y_pos=pong.field.y1 - 1;
        } else {
            pong.ball.y_pos=pong.field.y0 + 1;
        }
        pong.ball.y_speed*=-1;
    } else {
        // ball between top and bottom borders
        pong.ball.y_pos+=pong.ball.y_speed;
    }
}


/// @brief process player
void process_player() {

    if(!(((pong.player.y_pos + pong.player.y_speed + pong.player.length) > pong.field.y1) ||
       ((pong.player.y_pos + pong.player.y_speed) <= pong.field.y0))) {
        // stick between top and bottom borders
        pong.player.y_pos+= pong.player.y_speed;
    } else {
        if(pong.player.y_speed > 0) {
            // set stick to the lowest position
            pong.player.y_pos = pong.field.y1 - pong.player.length;
        } else {
            // set stick to the highest position
            pong.player.y_pos = pong.field.y0;
        }
    }
}


/// @brief process bot
void process_bot() {
    // change bot stick speed according to ball position 
    if((pong.bot.y_pos + 4) < pong.ball.y_pos) {
        pong.bot.y_speed = 4;
    } else if ((pong.bot.y_pos + 8) > pong.ball.y_pos) {
        pong.bot.y_speed = -4;
    } else {
        pong.bot.y_speed = 0;
    }
    // change bot stick position 
    if(!(((pong.bot.y_pos + pong.bot.y_speed + pong.bot.length) > pong.field.y1) ||
       ((pong.bot.y_pos + pong.bot.y_speed) < pong.field.y0))) {
        // bot stick between top and bottom border
        pong.bot.y_pos+= pong.bot.y_speed;
    } 
    else {
        if(pong.bot.y_speed > 0) {
            // set stick to the lowest position
            pong.bot.y_pos = pong.field.y1 - pong.bot.length;
        } else {
            // set stick to the highest position
            pong.bot.y_pos = pong.field.y0;
        }
    }
}


/// @brief Handle display
/// @param pvParameter task parameter
void display_task(void *pvParameter) {
    while(1) {
        // process player
        process_player();
        // update player stick position
        update_stick(pong.player);
        // process bot
        process_bot();
        // update bot stick position
        update_stick(pong.bot);
        // process ball
        process_ball();
        // update ball position
        update_ball(pong.ball);
        // delay to make space for other tasks to run
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}


/// @brief start the game
void start_game() {
    // init game objects 
    init_game_objects();
    // draw game objects
    draw_objects();
    // create display task
    xTaskCreate(&display_task, "display_task", 2048, NULL, 5, NULL);
    // create joystick task
    xTaskCreate(&joystick_task, "joystick_task", 2048, NULL, 5, NULL);
}