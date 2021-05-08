#include "main.h"

const float FPS = 100;
const int SCREEN_X = 1250;
const int SCREEN_Y = 615;

ALLEGRO_SAMPLE *soundtrack = NULL;
ALLEGRO_SAMPLE *clapping = NULL;
ALLEGRO_SAMPLE *boos = NULL;
ALLEGRO_SAMPLE_INSTANCE *instSoundtrack = NULL;
ALLEGRO_SAMPLE_INSTANCE *instClapping = NULL;
ALLEGRO_SAMPLE_INSTANCE *instBoos = NULL;

float dist(float x1, float x2, float y1, float y2)
{
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

typedef struct Ball
{
	float x;
	float y;
	float radius;
	float dx;
	float dy;
	ALLEGRO_COLOR color;
} Ball;

void ballCreation(Ball *b)
{
	b->color = al_map_rgb(rand() % 128, rand() % 128, rand() % 128);
	b->radius = rand() % 10;
	b->dx = pow(-1, rand() % 2);
	b->dy = pow(-1, rand() % 2);
	b->x = rand() % (SCREEN_X);
	b->y = rand() % (SCREEN_Y);
}

void ballDraw(Ball b)
{
	al_draw_filled_circle(b.x, b.y, b.radius, b.color);
}

void loseSize(Ball *discard, Ball B, float X, float Y)
{
	if (B.x + 5 >= X && B.y + 5 >= Y && B.x - 5 <= X && B.y - 5 <= Y)
	{
		discard->y = rand() % SCREEN_Y;
		discard->x = rand() % SCREEN_X;
		discard->dx = 0;
		discard->dy = 0;
	}
	else if (B.x < X && B.y + 5 >= Y && B.y - 5 <= Y)
	{
		discard->y = B.y;
		discard->x = B.x + B.radius + 6;
		discard->dx = -B.dx;
		discard->dy = 0;
	}
	else if (B.y - 5 > Y && B.x + 5 < X)
	{
		discard->y = B.y + B.radius - 6;
		discard->x = B.x + B.radius + 6;
		discard->dx = -B.dx;
		discard->dy = -B.dy;
	}
	else if (B.y > Y && B.x - 5 < X && B.x + 5 > X)
	{
		discard->y = B.y - B.radius - 6;
		discard->x = B.x;
		discard->dx = -B.dx;
		discard->dy = 0;
	}
	else if (B.y - 5 > Y && B.x - 5 > X)
	{
		discard->y = B.y - B.radius - 6;
		discard->x = B.x - B.radius - 6;
		discard->dx = -B.dx;
		discard->dy = -B.dy;
	}
	else if (B.x > X && B.y - 5 < Y && B.y + 5 > Y)
	{
		discard->y = B.y;
		discard->x = B.x - B.radius - 6;
		discard->dx = 0;
		discard->dy = -B.dy;
	}
	else if (B.y + 5 < Y && B.x - 5 > X)
	{
		discard->y = B.y + B.radius + 6;
		discard->x = B.x - B.radius - 6;
		discard->dx = -B.dx;
		discard->dy = -B.dy;
	}
	else if (B.y < Y && B.x - 5 < X && B.x + 5 > X)
	{
		discard->y = B.y + B.radius + 6;
		discard->x = B.x;
		discard->dx = -B.dx;
		discard->dy = 0;
	}
	else if (B.y + 5 < Y && B.x + 5 < X)
	{
		discard->y = B.y + B.radius + 6;
		discard->x = B.x + B.radius + 6;
		discard->dx = -B.dx;
		discard->dy = -B.dy;
	}
}

int main()
{
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_FONT *fonte = NULL;
	ALLEGRO_EVENT_QUEUE *eventQueue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	bool tocar_aplasusos = false;
	int playing = 1;
	float seg = 0, segaux = 0;
	float record = 0;

	FILE *score = fopen("record.txt", "r+");

	fscanf(score, "%f", &record);

	// Error messages :
	if (!al_init())
	{
		fprintf(stderr, "Allegro init error!\n");
		return -1;
	}
	if (!al_init_primitives_addon())
	{
		fprintf(stderr, "Primitives init error!\n");
		return -1;
	}

	display = al_create_display(SCREEN_X, SCREEN_Y);
	if (!display)
	{
		fprintf(stderr, "Display init error!\n");
		return -1;
	}

	timer = al_create_timer(1.0 / FPS);
	if (!timer)
	{
		fprintf(stderr, "Timer init error!\n");
		return -1;
	}

	eventQueue = al_create_event_queue();
	if (!eventQueue)
	{
		fprintf(stderr, "EventQueue init error!\n");
		return -1;
	}

	al_init_font_addon();
	if (!al_init_ttf_addon())
	{
		fprintf(stderr, "ttf_addon init error\n");
		return -1;
	}

	if (!al_install_mouse())
	{
		fprintf(stderr, "Mouse init error!\n");
	}

	fonte = al_load_font("arial.TTF", 48, 0);
	if (!fonte)
	{
		al_destroy_display(display);
		fprintf(stderr, "Font init error!\n");
		return -1;
	}

	//-----------------------------------audio------------------------------------------------------------

	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(10);

	soundtrack = al_load_sample("soundtrack.ogg");
	instSoundtrack = al_create_sample_instance(soundtrack);
	al_attach_sample_instance_to_mixer(instSoundtrack, al_get_default_mixer());
	al_set_sample_instance_playmode(instSoundtrack, ALLEGRO_PLAYMODE_LOOP);
	al_set_sample_instance_gain(instSoundtrack, 0.8);

	clapping = al_load_sample("clapping.ogg");
	instClapping = al_create_sample_instance(clapping);
	al_attach_sample_instance_to_mixer(instClapping, al_get_default_mixer());
	al_set_sample_instance_playmode(instClapping, ALLEGRO_PLAYMODE_LOOP);
	al_set_sample_instance_gain(instClapping, 0.8);

	boos = al_load_sample("boos.ogg");
	instBoos = al_create_sample_instance(boos);
	al_attach_sample_instance_to_mixer(instBoos, al_get_default_mixer());
	al_set_sample_instance_playmode(instBoos, ALLEGRO_PLAYMODE_LOOP);
	al_set_sample_instance_gain(instBoos, 0.8);

	//-----------------------------------------------------------------------------------------------

	// Register in the event queue that I want to identify when the screen has changed
	al_register_event_source(eventQueue, al_get_display_event_source(display));
	// Register in the event queue that I want to identify when the time has changed from t to t + 1
	al_register_event_source(eventQueue, al_get_timer_event_source(timer));
	// Register mouse in the event queue:
	al_register_event_source(eventQueue, al_get_mouse_event_source());

	ALLEGRO_COLOR BKG_COLOR = al_map_rgb(0, 0, 0);
	// Warns the glad that now I want to modify the properties of the screen
	al_set_target_bitmap(al_get_backbuffer(display));
	// Black bkg
	al_clear_to_color(BKG_COLOR);

	int i, numRandomBalls = 50, ballsCreated = 0;
	Ball player, enemy, comida[100];
	ballCreation(&player);
	ballCreation(&enemy);

	//Random balls cration
	for (i = 0; i <= numRandomBalls; i++)
	{
		ballCreation(&comida[i]);
	}

	//especificando radius, velocidade e color do enemy e player
	player.radius = 30;
	enemy.radius = 30;
	player.dx = 0;
	player.dy = 0;
	enemy.dx = 1;
	enemy.dy = 1;
	player.color = al_map_rgb(255, 255, 0);
	enemy.color = al_map_rgb(255, 0, 0);
	player.y = SCREEN_Y / 2;
	player.x = SCREEN_X / 2;
	enemy.x = SCREEN_X - 100;
	enemy.y = SCREEN_Y - 100;

	ALLEGRO_EVENT ev;

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_text(fonte, al_map_rgb(255, 255, 255), (SCREEN_X / 2) - 10, 30, ALLEGRO_ALIGN_CENTRE, " Comilator ");
	al_draw_text(fonte, al_map_rgb(255, 255, 255), 600, 150, ALLEGRO_ALIGN_CENTRE, ". Control the yellow ball by the click of the mouse");
	al_draw_text(fonte, al_map_rgb(255, 255, 255), 600, 220, ALLEGRO_ALIGN_CENTRE, ". Be careful, with each click your ball loses mass!");
	al_draw_text(fonte, al_map_rgb(255, 255, 255), 610, 290, ALLEGRO_ALIGN_CENTRE, ". You lose if any ball in the game has");
	al_draw_text(fonte, al_map_rgb(255, 255, 255), 600, 340, ALLEGRO_ALIGN_CENTRE, "radius bigger than yours");
	al_draw_text(fonte, al_map_rgb(255, 255, 255), 600, 410, ALLEGRO_ALIGN_CENTRE, ". To win, eat the enemy!");
	al_draw_text(fonte, al_map_rgb(255, 255, 0), 600, 490, ALLEGRO_ALIGN_CENTRE, "Good eating!");
	al_flip_display();
	al_rest(7.0);

	al_start_timer(timer);

	while (playing)
	{

		al_wait_for_event(eventQueue, &ev);

		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			playing = 0;
		}

		al_play_sample_instance(instSoundtrack);

		if (ev.type == ALLEGRO_EVENT_TIMER)
		{

			seg += (1.0) / FPS; // chronometer

			if (player.x > SCREEN_X || player.x < 0)
				player.dx = -player.dx;
			if (enemy.x > SCREEN_X || enemy.x < 0)
				enemy.dx = -enemy.dx;
			if (player.y > SCREEN_Y || player.y < 0)
				player.dy = -player.dy;
			if (enemy.y > SCREEN_Y || enemy.y < 0)
				enemy.dy = -enemy.dy;

			// End game
			if (dist(player.x, enemy.x, player.y, enemy.y) <= player.radius + enemy.radius && player.radius > enemy.radius)
			{
				al_destroy_sample(soundtrack);
				al_destroy_sample_instance(instSoundtrack);
				al_play_sample_instance(instClapping);
				segaux = seg;
				if (record > segaux || record == 0)
				{
					fclose(score);
					score = fopen("record.txt", "w");
					fprintf(score, "%f", segaux);
					al_clear_to_color(al_map_rgb(0, 0, 0));
					al_draw_text(fonte, al_map_rgb(255, 255, 255), (SCREEN_X / 2) - 10, (SCREEN_Y / 2) - 50, ALLEGRO_ALIGN_CENTRE, "win and new record");
					al_flip_display();
					al_rest(5.0);
					fclose(score);
					al_destroy_font(fonte);
					al_destroy_display(display);
					al_destroy_event_queue(eventQueue);
					al_destroy_sample(clapping);
					al_destroy_sample(boos);
					al_destroy_sample_instance(instClapping);
					al_destroy_sample_instance(instBoos);
					return 0;
				}
				else
				{
					al_clear_to_color(al_map_rgb(0, 0, 0));
					al_draw_text(fonte, al_map_rgb(255, 255, 255), (SCREEN_X / 2) - 10, (SCREEN_Y / 2) - 50, ALLEGRO_ALIGN_CENTRE, "win");
					al_flip_display();
					al_rest(5.0);
					fclose(score);
					al_destroy_event_queue(eventQueue);
					al_destroy_sample(boos);
					al_destroy_sample(clapping);
					al_destroy_sample_instance(instClapping);
					al_destroy_sample_instance(instBoos);
					al_destroy_font(fonte);
					al_destroy_display(display);
					return 0;
				}
			}
			if (dist(player.x, enemy.x, player.y, enemy.y) <= player.radius + enemy.radius && player.radius < enemy.radius || player.radius < 1)
			{
				al_destroy_sample(soundtrack);
				al_destroy_sample_instance(instSoundtrack);
				al_play_sample_instance(instBoos);
				al_clear_to_color(al_map_rgb(0, 0, 0));
				al_draw_text(fonte, al_map_rgb(255, 255, 255), (SCREEN_X / 2) - 10, (SCREEN_Y / 2) - 50, ALLEGRO_ALIGN_CENTRE, "you lost");
				al_flip_display();
				al_rest(5.0);
				fclose(score);
				al_destroy_font(fonte);
				al_destroy_display(display);
				al_destroy_event_queue(eventQueue);
				al_destroy_sample(boos);
				al_destroy_sample(clapping);
				al_destroy_sample_instance(instClapping);
				al_destroy_sample_instance(instBoos);
				return 0;
			}

			// Random balls actions
			for (i = 0; i <= numRandomBalls + ballsCreated; i++)
			{
				if (comida[i].x > SCREEN_X || comida[i].x < 0)
					comida[i].dx = -comida[i].dx;
				if (comida[i].y > SCREEN_Y || comida[i].y < 0)
					comida[i].dy = -comida[i].dy;

				if (dist(player.x, comida[i].x, player.y, comida[i].y) <= player.radius + comida[i].radius && player.radius >= comida[i].radius)
				{
					player.radius = sqrt(pow(player.radius, 2) + pow(comida[i].radius, 2));
					comida[i].radius = rand() % 10;
					comida[i].dx = pow(-1, rand() % 2);
					comida[i].dy = pow(-1, rand() % 2);
					comida[i].x = rand() % (SCREEN_X - 10);
					comida[i].y = rand() % (SCREEN_Y - 10);
				}

				// if the food is bigger than the player
				if (dist(player.x, comida[i].x, player.y, comida[i].y) <= player.radius + comida[i].radius && player.radius < comida[i].radius)
				{
					al_destroy_sample(soundtrack);
					al_destroy_sample_instance(instSoundtrack);
					al_play_sample_instance(instBoos);
					al_clear_to_color(al_map_rgb(0, 0, 0));
					al_draw_text(fonte, al_map_rgb(255, 255, 255), (SCREEN_X / 2) - 10, (SCREEN_Y / 2) - 50, ALLEGRO_ALIGN_CENTRE, "you lost");
					al_flip_display();
					al_rest(5.0);
					fclose(score);
					al_destroy_font(fonte);
					al_destroy_display(display);
					al_destroy_event_queue(eventQueue);
					al_destroy_sample(boos);
					al_destroy_sample(clapping);
					al_destroy_sample_instance(instClapping);
					al_destroy_sample_instance(instBoos);
					return 0;
				}
				if (dist(enemy.x, comida[i].x, enemy.y, comida[i].y) <= enemy.radius + comida[i].radius && enemy.radius < comida[i].radius)
				{
					al_clear_to_color(al_map_rgb(0, 0, 0));
					al_draw_text(fonte, al_map_rgb(255, 255, 255), (SCREEN_X / 2) - 10, (SCREEN_Y / 2) - 50, ALLEGRO_ALIGN_CENTRE, "you win. enemy bobao!");
					al_flip_display();
					al_rest(5.0);
					fclose(score);
					al_destroy_event_queue(eventQueue);
					al_destroy_sample(boos);
					al_destroy_sample(clapping);
					al_destroy_sample_instance(instClapping);
					al_destroy_sample_instance(instBoos);
					al_destroy_font(fonte);
					al_destroy_display(display);
					return 0;
				}

				// enemy eat food
				if (dist(enemy.x, comida[i].x, enemy.y, comida[i].y) <= enemy.radius + comida[i].radius && enemy.radius >= comida[i].radius)
				{
					enemy.radius = sqrt(pow(enemy.radius, 2) + pow(comida[i].radius, 2));
					comida[i].radius = rand() % 10;
					comida[i].dx = pow(-1, rand() % 2);
					comida[i].dy = pow(-1, rand() % 2);
					comida[i].x = rand() % (SCREEN_X - 10);
					comida[i].y = rand() % (SCREEN_Y - 10);
				}

				// making the food go
				comida[i].x += (comida[i].dx);
				comida[i].y += (comida[i].dy);

				// balls if they eat
				int j;
				for (j = 0; j <= numRandomBalls + ballsCreated; j++)
				{
					if (dist(comida[i].x, comida[j].x, comida[i].y, comida[j].y) <= comida[i].radius + comida[j].radius && comida[i].radius > comida[j].radius)
					{
						comida[i].radius = sqrt(pow(comida[i].radius, 2) + pow(comida[j].radius, 2));
						comida[j].radius = rand() % 10;
						comida[j].dx = pow(-1, rand() % 2);
						comida[j].dy = pow(-1, rand() % 2);
						comida[j].x = rand() % (SCREEN_X - 10);
						comida[j].y = rand() % (SCREEN_Y - 10);
					}
				}
			}

			// player and enemy walk
			player.x += player.dx;
			enemy.x += enemy.dx;
			player.y += player.dy;
			enemy.y += enemy.dy;
			al_clear_to_color(BKG_COLOR);

			ballDraw(player);
			ballDraw(enemy);
			for (i = 0; i <= (numRandomBalls + ballsCreated); i++)
			{
				ballDraw(comida[i]);
			}
			al_draw_textf(fonte, al_map_rgb(255, 255, 255), 350, 20, ALLEGRO_ALIGN_CENTRE, "Time: %.2f   Record: %.2f", seg, record);
			al_flip_display();
		}

		// commands related to mouse click
		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
		{

			// two lines to change player speed with click
			player.dx += (-(ev.mouse.x - player.x) / dist(ev.mouse.x, player.x, ev.mouse.y, player.y));
			player.dy += (-(ev.mouse.y - player.y) / dist(ev.mouse.x, player.x, ev.mouse.y, player.y));

			// decreasing the size of the player after the click and expelling a new ball with an area equal to the reduced area of the player.
			ballsCreated++;
			loseSize(&comida[numRandomBalls + ballsCreated], player, ev.mouse.x, ev.mouse.y);

			comida[numRandomBalls + ballsCreated].color = al_map_rgb(255, 255, 0);
			comida[numRandomBalls + ballsCreated].radius = player.radius * 0, 3; // -(sqrt(pow(player.radius, 2)- pow(5,2)));
			player.radius = sqrt(pow(player.radius, 2) - pow(5, 2));

			// to avoid the bug when there are more than 100 balls on the screen
			if (ballsCreated == 50)
				ballsCreated = 0;
		}
	}
	return 0;
}
