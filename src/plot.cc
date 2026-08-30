#include "plot.h"
#include "common.h"
#include "console.h"
#include "raylib.h"
#include "raymath.h"
#include "solver.h"
#include <cassert>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "parser.h"
#include "io.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


#define DEBUG if (1)

const int _MAX_VALUEBOX_STR = 100;

const int _SCREEN_WIDTH = 700;
const int _SCREEN_HEIGHT = 700;

const int _GRID_LINES_WIDTH = 2;
const int _GRID_TEXT_PADDING = 5;
const int _GRID_FONT_SIZE = 16;

const int _NUM_POINTS = 100;
const int _MAX_ROOT_POINTS = 3;
const int _ROOT_POINTS_RADIUS = 6;

const int _CURSOR_POINT_RADIUS = 4;
const int _CURSOR_TEXT_PADDING_VERTICAL = -24;
const int _CURSOR_TEXT_PADDING_HORIZONTAL = -44;
const int _CURSOR_MAGNET_DISTANCE = 7;

const float _DEFAULT_ZOOM = 40;
const Vector2 _DEFAULT_GRID_POS = {_SCREEN_WIDTH/2., _SCREEN_HEIGHT/2.};
const Vector2 _DEFAULT_GRID_STEP = {1., 1.};
const float _MIN_GRID_STEP = 1e-2;
const float _MAX_GRID_STEP = 1e2;
const float _ZOOM_CHANGE_FACTOR = 2.1;
const float _MIN_GRID_ZOOM = 10;
const float _MAX_GRID_ZOOM = 1000;

const float _BUTTON_SIZE = 50.;
const float _BUTTON_PADDING = 7;
const float _FIELD_LENGTH = 150.;
const float _FIELD_HEIGHT = 40.;
const int _GUI_FONT_SIZE = 20;

#define _DEFAULT_EXPRESSION "x^2";

//---------------------------------------------------
const Rectangle settingsRect = 
	{_SCREEN_WIDTH - _BUTTON_PADDING - _BUTTON_SIZE,
 	_SCREEN_HEIGHT - _BUTTON_PADDING - _BUTTON_SIZE,
 	_BUTTON_SIZE, _BUTTON_SIZE};

const Rectangle homeRect = 
	{_SCREEN_WIDTH - _BUTTON_PADDING - _BUTTON_SIZE, 
 	_BUTTON_PADDING,
 	_BUTTON_SIZE, _BUTTON_SIZE};

const Rectangle plusRect = 
	{_SCREEN_WIDTH - _BUTTON_PADDING - _BUTTON_SIZE, 
 	homeRect.y + _BUTTON_SIZE + _BUTTON_PADDING,
 	_BUTTON_SIZE, _BUTTON_SIZE};

const Rectangle minusRect = 
	{_SCREEN_WIDTH - _BUTTON_PADDING - _BUTTON_SIZE, 
 	plusRect.y + _BUTTON_SIZE + _BUTTON_PADDING,
 	_BUTTON_SIZE, _BUTTON_SIZE};

const Rectangle settingsWindowRect = 
	{_BUTTON_PADDING, _BUTTON_PADDING,
 	_FIELD_LENGTH + _BUTTON_PADDING * 2, 
 	_FIELD_HEIGHT * 6 + _BUTTON_PADDING * 7};

const Rectangle settingsXLabelRect = 
	{settingsWindowRect.x + _BUTTON_PADDING, 
	settingsWindowRect.y + _BUTTON_PADDING * 3,
	_FIELD_LENGTH, _FIELD_HEIGHT};

const Rectangle settingsXStepRect = 
	{settingsWindowRect.x + _BUTTON_PADDING, 
	settingsXLabelRect.y + _FIELD_HEIGHT,
	_FIELD_LENGTH, _FIELD_HEIGHT};

const Rectangle settingsYLabelRect = 
	{settingsWindowRect.x + _BUTTON_PADDING, 
	settingsXStepRect.y + _FIELD_HEIGHT + _BUTTON_PADDING,
	_FIELD_LENGTH, _FIELD_HEIGHT};

const Rectangle settingsYStepRect = 
	{settingsWindowRect.x + _BUTTON_PADDING, 
	settingsYLabelRect.y + _FIELD_HEIGHT,
	_FIELD_LENGTH, _FIELD_HEIGHT};

const Rectangle settingsZLabelRect = 
	{settingsWindowRect.x + _BUTTON_PADDING, 
	settingsYStepRect.y + _FIELD_HEIGHT + _BUTTON_PADDING,
	_FIELD_LENGTH, _FIELD_HEIGHT};

const Rectangle settingsZRect = 
	{settingsWindowRect.x + _BUTTON_PADDING, 
	settingsZLabelRect.y + _FIELD_HEIGHT,
	_FIELD_LENGTH, _FIELD_HEIGHT};

const Rectangle exprRext = 
	{_BUTTON_PADDING* 2,
	_SCREEN_HEIGHT - _BUTTON_SIZE - _BUTTON_PADDING,
	_SCREEN_WIDTH - 2 * _BUTTON_SIZE,
	_BUTTON_SIZE};
//------------------------------------------------------

//Global variables. Will be initialized on _LoadResources. DO NOT USE!
Font __NUMBERS_FONT;
Font __GUI_FONT;
Image __ICON;

struct Grid{
	Vector2 pos;
	Vector2 step;
	float zoom;
};

/**
 * Load resources: pictures, fonts and other
 */
void _LoadResources();

/**
 *	Draws grid in the window 
 * 	@param [in] grid pointer to grid struct
 */
void _DrawGrid(const Grid* grid);

/**
 *	Draws coordinate axes in the window 
 * 	@param [in] grid pointer to grid struct
 *  @param [in] font font for labels
 */
void _DrawAxes(const Grid* grid);

/**
 * Convert coordinates from screen to grid space
 */
Vector2 _ScreenToGrid(const Grid* g, Vector2 p);

/**
 * Convert coordinates from grid to screen space
 */
Vector2 _GridToScreen(const Grid* g, Vector2 p);

/**
 * Get bounds for drawing [x_begin, x_end] - the range of x-values that fall within the screen area
 * @param [in] grid 
 * @param [in] equation coeffs
 * @returns The range of x-values that fall within the screen area
 * @note In case the graph does not fit on the screen returns {NAN, NAN}!
 */
Vector2 _GetBounds(const Grid* grid, struct EquationCoeffs coeffs);

/**
 * Draws function on the screen
 */
void _DrawFunction(const Grid* grid, 
				   /*double (*GetVal)(const struct EquationCoeffs* coeffs,double val),*/
				   const struct EquationCoeffs* coeffs, Vector2 bounds);

/**
 * Zoom into grid (decrease step, increase zoom)
 */
void _ZoomIn(Grid* g);

/**
 * Zoom out of grid (decrease step, increase zoom)
 */
void _ZoomOut(Grid* g);

//! Reset zoom and step
void _ResetGrid(Grid* g);

//! Draw settings window
bool _ProcessGridSettingsWindow(Grid* grid);

//! Fix and returns grid steps if incorrect
Vector2 _CorrectGridStep(Vector2 step);

//! Fix and returns grid zoom if incorrect
float _CorrectGridZoom(float zoom);

//! Draws mouse highlight
void _DrawMouseHighlight();

//! Update roots and root points
void _UpdateRootsAndPoints(struct EquationCoeffs* coeffs, struct EquationSolutions* roots, Vector2 root_points[_MAX_ROOT_POINTS]);
//--------------------------------------------
void _LoadResources(){
	__NUMBERS_FONT = LoadFontEx("res/SFProText-Medium.ttf", _GRID_FONT_SIZE, 0, 250);
	__GUI_FONT = LoadFontEx("res/SFProText-Medium.ttf", _GUI_FONT_SIZE, 0, 250);
	__ICON = LoadImage("res/x2.png");

	GuiSetFont(__GUI_FONT);
    GuiSetStyle(DEFAULT, TEXT_SIZE, _GUI_FONT_SIZE);
   	GuiSetStyle(VALUEBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
   	GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

	assert(IsFontValid(__NUMBERS_FONT) && "Failed to load font!");
	assert(IsImageValid(__ICON) && "Failed to load font!");
}

Vector2 _ScreenToGrid(const Grid* g, Vector2 p){
	return Vector2{
		.x = (p.x - g->pos.x) / g->zoom,
		.y = (g->pos.y - p.y) / g->zoom
	};
}

Vector2 _GridToScreen(const Grid* g, Vector2 p){
	return Vector2{
		.x = (g->pos.x + p.x * g->zoom),
		.y = (g->pos.y - p.y * g->zoom)
	};
}

void _DrawAxes(const Grid* grid){
	Vector2 topLeft = _ScreenToGrid(grid, {0, 0});
	Vector2 bottomRight = _ScreenToGrid(grid, {_SCREEN_WIDTH, _SCREEN_HEIGHT});
	Vector2 center_scr = _GridToScreen(grid, {0, 0});
	bool x_on_top = false;
	bool x_on_bot = false;
	bool y_on_left = false;
	bool y_on_right = false;

	if (center_scr.x < 0) y_on_left = true;
	if (center_scr.x > _SCREEN_WIDTH) y_on_right = true;
	if (center_scr.y < 0) x_on_top = true;
	if (center_scr.y > _SCREEN_HEIGHT) x_on_bot = true;
	//Draw x --->
	if (!x_on_bot && !x_on_top){
		DrawLineEx({0, 		 	 	 grid->pos.y}, 
			       {_SCREEN_WIDTH, 	 grid->pos.y}, 
			       2, 
			       BLACK);
	}
	//Draw x labels
	float x_start = floor(topLeft.x / grid->step.x) * grid->step.x;
	float x_end = floor(bottomRight.x / grid->step.x) * grid->step.x;
	float pos = x_start;

	float x_labels_pos = grid->pos.y + _GRID_TEXT_PADDING;
	if (x_on_top) x_labels_pos = _GRID_TEXT_PADDING;
	if (x_on_bot) x_labels_pos = _SCREEN_HEIGHT - _GRID_TEXT_PADDING - _GRID_FONT_SIZE;

	while(pos<=x_end){
		if (!(fabs(pos)<1e-9)){
			DrawTextPro(__NUMBERS_FONT, 
					   TextFormat("%.2g", pos),
					   {grid->pos.x + pos * grid->zoom, 
					   x_labels_pos},
					   {0, 0},
					   90,
					   _GRID_FONT_SIZE,
					   0,
					   !x_on_top && !x_on_bot ? BLACK : DARKBLUE
					   );
		}
		pos += grid->step.x;
	}
	//Draw y |^
	if (!y_on_left && !y_on_right){
		DrawLineEx({grid->pos.x,     0}, 
			       {grid->pos.x, 	 _SCREEN_HEIGHT}, 
			       2, 
			       BLACK);
	}
	//Draw y labels
	float y_start = floor(bottomRight.y / grid->step.y) * grid->step.y;
	float y_stop = floor(topLeft.y / grid->step.y) * grid->step.y;
	pos = y_start;

	float y_labels_pos = grid->pos.x + _GRID_TEXT_PADDING;
	if (y_on_left) y_labels_pos = _GRID_TEXT_PADDING;
	if (y_on_right) y_labels_pos = _SCREEN_WIDTH - _GRID_TEXT_PADDING - _GRID_FONT_SIZE;

	while(pos <= y_stop){
		DrawTextPro(__NUMBERS_FONT, 
					TextFormat("%.2g", pos),
					{y_labels_pos,
					grid->pos.y - pos * grid->zoom},
					{0, 0},
					0,
					_GRID_FONT_SIZE,
					0,
					!y_on_left && !y_on_right ? BLACK : DARKBLUE
					);
		pos += grid->step.y;
	}

}

void _DrawGrid(const Grid* grid){
	assert(grid != NULL);

	Vector2 topLeft = _ScreenToGrid(grid, {0, 0});
	Vector2 bottomRight = _ScreenToGrid(grid, {_SCREEN_WIDTH, _SCREEN_HEIGHT});
	//DEBUG printf("\ntopLeft: %f %f\n", topLeft.x, topLeft.y);

	//Draw vertical |
	float x_start = floor(topLeft.x / grid->step.x) * grid->step.x;
	float x_end = floor(bottomRight.x / grid->step.x) * grid->step.x;
	float pos = x_start;
	while(pos <= x_end){
		if (!(fabs(pos) < 1e-9)){
			float x_scr = grid->pos.x + pos * grid->zoom;
			DrawLineEx({x_scr, 0}, 
			       	   {x_scr, _SCREEN_HEIGHT}, 
			       	   _GRID_LINES_WIDTH, 
			       	   LIGHTGRAY);
		}	
		pos += grid->step.x;
	}
	//Draw horizontal -
	float y_start = floor(bottomRight.y / grid->step.y) * grid->step.y;
	float y_stop = floor(topLeft.y / grid->step.y) * grid->step.y;
	pos = y_start;
	while(pos <= y_stop){
		if (!(fabs(pos) < 1e-9)){
			float y_scr = grid->pos.y - pos * grid->zoom;
			DrawLineEx({0, 		 	 y_scr}, 
			       	   {_SCREEN_WIDTH, y_scr}, 
			       _GRID_LINES_WIDTH, 
			       LIGHTGRAY);
		}
		pos += grid->step.y;
	}

}

//returns {nan, nan} if graph not in viewport
Vector2 _GetBounds(const Grid* grid, struct EquationCoeffs coeffs){
	assert(grid != NULL);

	Vector2 topLeft = _ScreenToGrid(grid, {0, 0});
	Vector2 bottomRight = _ScreenToGrid(grid, {_SCREEN_WIDTH, _SCREEN_HEIGHT});
	Vector2 bounds = {}; //(left/right)
	if (IsZero(coeffs.coeff_of_sq_x)) return {topLeft.x, bottomRight.x};
	if (coeffs.coeff_of_sq_x > 0){
		coeffs.free_coeff -= topLeft.y;
	}else{
		coeffs.free_coeff -= bottomRight.y;
	}
	EquationSolutions roots;
	SolveEquation(&coeffs, &roots);
	//DEBUG PrintRoots(&roots, 2);
	//DEBUG printf("Левая граница экрана - %g правая - %g\n", topLeft.x, bottomRight.x);
	double mnroot = MIN(roots.root1, roots.root2);
	if (mnroot < topLeft.x) bounds.x = topLeft.x;
	else bounds.x = mnroot;
	double mxroot = MAX(roots.root1, roots.root2);
	if (mxroot > bottomRight.x) bounds.y = bottomRight.x;
	else bounds.y = mxroot;
	//DEBUG printf("Левый - %g правый - %g\n", bounds.x, bounds.y);
	if (Equals(bounds.x, bounds.y) || bounds.x > bounds.y) return {NAN, NAN};
	return bounds;
}

//TODO?
void _DrawFunction(const Grid* grid, 
				   /*double (*GetVal)(const struct EquationCoeffs* coeffs,double val),*/
				   const struct EquationCoeffs* coeffs, Vector2 bounds){
	assert(grid != NULL);
	assert(coeffs!= NULL);
	//assert(GetVal != NULL);

	float pos_x = bounds.x;
	Vector2 prev = {pos_x, (float)GetQuadraticValue(coeffs, pos_x)};
	Vector2 point = prev;
	float step = (bounds.y - bounds.x) / _NUM_POINTS; 
	if (IsZero(step)) return;
	while (pos_x <= bounds.y+1){ //TODO make const
		DrawLineEx(_GridToScreen(grid, prev), _GridToScreen(grid, point), 3, GREEN);
		prev = point;
		pos_x += step;
		point = {pos_x, (float)GetQuadraticValue(coeffs, pos_x)};
	}
}

void _ZoomIn(Grid* g){
	if (g->step.x > _MIN_GRID_STEP && g->step.y > _MIN_GRID_STEP && g->zoom < _MAX_GRID_ZOOM){
        		g->zoom *= _ZOOM_CHANGE_FACTOR;
        		g->step.x /= 2;
        		g->step.y /= 2;  		
    }
}

void _ZoomOut(Grid* g){
	if (g->step.x < _MAX_GRID_STEP && g->step.y < _MAX_GRID_STEP &&
		g->zoom > _MIN_GRID_ZOOM ){
        g->zoom /= _ZOOM_CHANGE_FACTOR;
        g->step.x *= 2;
        g->step.y *= 2;
    }
}

void _ResetGrid(Grid* g){
	g->pos = _DEFAULT_GRID_POS;
	g->step = _DEFAULT_GRID_STEP;
	g->zoom = _DEFAULT_ZOOM;
}

Vector2 _CorrectGridStep(Vector2 step){
	if (step.x < _MIN_GRID_STEP) step.x = _MIN_GRID_STEP;
	if (step.y < _MIN_GRID_STEP) step.y = _MIN_GRID_STEP;

	if (step.x > _MAX_GRID_STEP) step.x = _MAX_GRID_STEP;
	if (step.y > _MAX_GRID_STEP) step.y = _MAX_GRID_STEP;

	if (!isfinite(step.x) || !isfinite(step.y)){
		DEBUG printf("Non finite %g %g\n", step.x, step.y);
		step = _DEFAULT_GRID_STEP;
	}
	return step;
}

float _CorrectGridZoom(float zoom){
	if (!isfinite(zoom)) zoom = _DEFAULT_ZOOM;
	if(zoom < _MIN_GRID_ZOOM) zoom = _MIN_GRID_ZOOM;
	if(zoom > _MAX_GRID_ZOOM) zoom = _MAX_GRID_ZOOM;
	return zoom;
}

bool _ProcessGridSettingsWindow(Grid* g, bool* external_update_step, bool* external_update_zoom){
	bool state = GuiWindowBox(settingsWindowRect, "Setting");
	static bool _x_valueBoxEditMode = false, _y_valueBoxEditMode = false, _zoom_valueBoxEditMode = false;
	static char __do_not_use_x_str_val[_MAX_VALUEBOX_STR] = "1";
	static char __do_not_use_y_str_val[_MAX_VALUEBOX_STR] = "1";
	static Vector2 st = _DEFAULT_GRID_STEP;
	static float zoom = _DEFAULT_ZOOM;

	static char __do_not_use_zoom_str_val[_MAX_VALUEBOX_STR] = "1";

	if (*external_update_step){
		snprintf(__do_not_use_x_str_val, _MAX_VALUEBOX_STR, "%.2f", g->step.x);
		snprintf(__do_not_use_y_str_val, _MAX_VALUEBOX_STR, "%.2f", g->step.y);
		st.x = g->step.x;
		st.y = g->step.y;
		*external_update_step = false;
	}

	if (*external_update_zoom){
		snprintf(__do_not_use_zoom_str_val, _MAX_VALUEBOX_STR, "%.2f", g->zoom);
		zoom = g->zoom;
		*external_update_zoom = false;
	}

	GuiLabel(settingsXLabelRect, "X-axis step");
	if (GuiValueBoxFloat(settingsXStepRect, NULL, __do_not_use_x_str_val, &st.x, _x_valueBoxEditMode)
		&& strlen(__do_not_use_x_str_val) != 0){
		 _x_valueBoxEditMode = !_x_valueBoxEditMode;
		 g->step = _CorrectGridStep(st);
	}
	GuiLabel(settingsYLabelRect, "Y-axis step");
	if (GuiValueBoxFloat(settingsYStepRect, NULL, __do_not_use_y_str_val, &st.y, _y_valueBoxEditMode)
		&& strlen(__do_not_use_y_str_val) != 0){
		_y_valueBoxEditMode = !_y_valueBoxEditMode;
		g->step = _CorrectGridStep(st);
	}

	GuiLabel(settingsZLabelRect, "Zoom");
	if (GuiValueBoxFloat(settingsZRect, NULL, __do_not_use_zoom_str_val, &zoom, _zoom_valueBoxEditMode)
		&& strlen(__do_not_use_zoom_str_val) != 0){
		 _zoom_valueBoxEditMode = !_zoom_valueBoxEditMode;
		 g->zoom = _CorrectGridZoom(zoom);
	}

	return state;
}

void _DrawMouseHighlight(const Grid* g){
	DrawCircleV(GetMousePosition(), _CURSOR_POINT_RADIUS, DARKGRAY);
    Vector2 mouseOnGrid = _ScreenToGrid(g, GetMousePosition());
    DrawTextEx(__NUMBERS_FONT, TextFormat("[%.2f, %.2f]", mouseOnGrid.x, mouseOnGrid.y),
            Vector2Add(GetMousePosition(), 
            (Vector2){ _CURSOR_TEXT_PADDING_HORIZONTAL, _CURSOR_TEXT_PADDING_VERTICAL}), 
            _GRID_FONT_SIZE, 2, BLACK);
}

void _UpdateRootsAndPoints(struct EquationCoeffs* coeffs, struct EquationSolutions* roots, Vector2 root_points[_MAX_ROOT_POINTS]){
 	SolveEquation(coeffs, roots);

 	switch (roots->nRoots){
 	case ONE_ROOT:
 		root_points[0] = {(float)roots->root1, 0};
 		break;
 	case TWO_ROOTS:
 		root_points[0] = {(float)roots->root1, 0};
 		root_points[1] = {(float)roots->root2, 0};
 		break;
 	default:
 		break;
 	}
}

void GraphMode(){

	struct EquationCoeffs coeffs = {1, 0, 0};

	Grid g = {};
	_ResetGrid(&g);

	SetConfigFlags(FLAG_MSAA_4X_HINT);

	InitWindow(_SCREEN_WIDTH, _SCREEN_HEIGHT, "Quadratic");

	_LoadResources();

	SetWindowIcon(__ICON);

    SetTargetFPS(60);

    Vector2 bounds = _GetBounds(&g, coeffs);

 	bool settingsWindowOpened = false;
 	bool update_step = true;
 	bool update_zoom = true;
 	bool exprExitMode = false;

 	char expression[MAX_EXPRESSION_LENGTH] = _DEFAULT_EXPRESSION;
 	
 	EquationSolutions roots = {0, 0, NO_ROOTS};
 	SolveEquation(&coeffs, &roots);

 	Vector2 root_points[_MAX_ROOT_POINTS] = {};

 	_UpdateRootsAndPoints(&coeffs, &roots, root_points);

    while (!WindowShouldClose()){
    	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 delta = GetMouseDelta();
           	g.pos = Vector2Add(g.pos, delta);
        }

        bounds = _GetBounds(&g, coeffs);
    	BeginDrawing();
    		ClearBackground(RAYWHITE);
    		_DrawGrid(&g);
   			_DrawAxes(&g);
    		if (isfinite(bounds.x) && isfinite(bounds.y)){
    			_DrawFunction(&g, /*&GetQuadraticValue,*/ &coeffs, bounds);
    		}	

    		for(int i = 0; i < roots.nRoots; i++){
    			Vector2 screen_root_point = _GridToScreen(&g, root_points[i]);
    			DrawCircleV(screen_root_point, _ROOT_POINTS_RADIUS, RED);
    			if(Vector2Distance(screen_root_point, GetMousePosition()) < _CURSOR_MAGNET_DISTANCE){
    				SetMousePosition(screen_root_point.x, screen_root_point.y);
    			}
    		}

    		_DrawMouseHighlight(&g);

            if (GuiButton(homeRect, GuiIconText(ICON_HOUSE, ""))){
            	_ResetGrid(&g);
            	update_step = true;
	        	update_zoom = true;
            }

            if (GuiButton(settingsRect, GuiIconText(ICON_GEAR, ""))){
            	settingsWindowOpened = true;
            }

            if(settingsWindowOpened){
				settingsWindowOpened = !_ProcessGridSettingsWindow(&g, &update_step, &update_zoom);	
			}

	        if (IsKeyPressed(KEY_UP) || GuiButton(plusRect, GuiIconText(ICON_BOX_MORE, ""))){
	        	_ZoomIn(&g);
	        	update_step = true;
	        	update_zoom = true;
	        }
	        if (IsKeyPressed(KEY_DOWN) || GuiButton(minusRect, GuiIconText(ICON_BOX_MINUS, ""))){
	        	_ZoomOut(&g);
	        	update_step = true;
	        	update_zoom = true;
	        }

	        DrawRectangleRec(exprRext, RAYWHITE);
	        if (GuiTextBox(exprRext, expression, _GUI_FONT_SIZE, exprExitMode)){
	        	exprExitMode = !exprExitMode;

	        	enum ParsingStatus stat = ParseExpression(expression, &coeffs);
	        	if (stat == PARSING_ERROR){
	        		strcpy(expression, "Invalid expression");
	        	}
	       		_UpdateRootsAndPoints(&coeffs, &roots, root_points);

	        }
    	EndDrawing();
    }
}
