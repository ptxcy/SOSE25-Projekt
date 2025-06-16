#include "ui.h"


// ----------------------------------------------------------------------------------------------------
// Button Utility

/**
 *	update button intersection & states
 */
void Button::update()
{
	bool __Intersect = bounds.intersect(g_Input.mouse.position);

	// button confirmation
	if (__Intersect&&g_Input.mouse.buttons[0])
	{
		g_Renderer.assign_sprite_texture(canvas,action);
		holding = true;
		return;
	}

	// button hover
	if (__Intersect)
	{
		g_Renderer.assign_sprite_texture(canvas,hover);
		confirm = holding;
	}

	// reset button state
	else g_Renderer.assign_sprite_texture(canvas,idle);
	holding = false;
}

/**
 *	delete button
 */
void Button::remove()
{
	Renderer::delete_sprite(canvas);
	g_Renderer.delete_text(label);
}


// ----------------------------------------------------------------------------------------------------
// Text Field Utility

/**
 *	update textfield intersection & status
 *	\param font: pointer to batch font
 *	\param cursor: cursor entity to place by selected text field
 *	\param field_switch: true when text fields have been switched without in-between deselect
 */
void TextField::update(Font* font,Sprite* cursor,bool& field_switch)
{
	bool __Intersect = bounds.intersect(g_Input.mouse.position);

	// user requests to write to text field buffer & clickout handling
	if (active)
	{
		// handle selection & buffer
		if (!__Intersect&&g_Input.mouse.buttons[0])
		{
			if (!field_switch) Input::unset_input_mode();
			active = false;
			return;
		}
		else
		{
			content->data = (!hidden) ? buffer : string(buffer.size(),'*');
			content->load_buffer();
		}

		// place cursor when selected
		f32 __Offset = font->estimate_wordlength(content->data);
		cursor->offset = content->position+vec2(__Offset,0);
		cursor->scale.y = font->size*content->scale;
		COMM_LOG("%f %f",cursor->scale.x,cursor->scale.y);
	}
	else if (__Intersect&&g_Input.mouse.buttons[0])
	{
		g_Renderer.assign_sprite_texture(canvas,select);
		g_Input.set_input_mode(&buffer);
		active = true;
		field_switch = true;
	}

	// handle hover over inactive text field & reset to idle if no intersection
	else if (__Intersect) g_Renderer.assign_sprite_texture(canvas,hover);
	else g_Renderer.assign_sprite_texture(canvas,idle);
}

/**
 *	delete text field
 */
void TextField::remove()
{
	Renderer::delete_sprite(canvas);
	g_Renderer.delete_text(content);
}


// ----------------------------------------------------------------------------------------------------
// Button Entity

/**
 *	add a button to the batch
 *	\param label: button label writing across button surface
 *	\param tidle: idle texture, in case button is not interacted with this will be displayed
 *	\param thover: hover texture, in case mouse cursor hovers over button and button is not clicked
 *	\param taction: action texture, in case button is clicked and held
 *	\param position: center position of button
 *	\param scale: dimensions of the button
 *	\param alignment: (default neutral fullscreen) screen alignment within given borders
 *	\returns address of button to later read interaction state from
 */
lptr<Button> UIBatch::add_button(const char* label,PixelBufferComponent* tidle,PixelBufferComponent* thover,
								 PixelBufferComponent* taction,vec2 position,vec2 scale,Alignment alignment)
{
	// graphics setup
	buttons.push_back({
			.idle = tidle,
			.hover = thover,
			.action = taction
		});
	lptr<Button> p_Button = std::prev(buttons.end());
	p_Button->canvas = g_Renderer.register_sprite(p_Button->idle,position,scale,.0f,alpha,alignment);

	// label text
	p_Button->label = g_Renderer.write_text(font,label,p_Button->canvas->offset,scale.y*.6f,vec4(1));

	// intersection boundaries
	vec2 __HScale = scale*vec2(.5f);
	p_Button->bounds = {
		.position = p_Button->canvas->offset-__HScale,
		.extent = p_Button->canvas->offset+__HScale
	};
	return p_Button;
}

/**
 *	add a text field to the batch
 *	\param tidle: idle texture, in case text field is not interacted with this will be displayed
 *	\param thover: hover texture, in case mouse cursor hovers over text field and text field is not clicked
 *	\param tselect: selection text when text field is clicked and user is writing to it
 *	\param position: center position of the text field
 *	\param scale: dimensions of the text field
 *	\param alignment: (default neutral fullscreen) screen alignment within given borders
 *	\returns address of text field to later read written contents from
 */
lptr<TextField> UIBatch::add_text_field(PixelBufferComponent* tidle,PixelBufferComponent* thover,
										PixelBufferComponent* tselect,vec2 position,vec2 scale,
										Alignment alignment)
{
	// graphics setup
	tfields.push_back({
			.idle = tidle,
			.hover = thover,
			.select = tselect
		});
	lptr<TextField> p_TextField = std::prev(tfields.end());
	p_TextField->canvas = g_Renderer.register_sprite(p_TextField->idle,position,scale,.0f,alpha,alignment);

	// setup content draw
	vec2 __HScale = scale*vec2(.5f);
	vec2 __TPos = p_TextField->canvas->offset-vec2(__HScale.x*.95f,__HScale.y*.4f);
	p_TextField->content = g_Renderer.write_text(font,"",__TPos,scale.y*.6,
												 vec4(1),{ .align=SCREEN_ALIGN_BOTTOMLEFT });

	// intersection boundaries
	p_TextField->bounds = {
		.position = p_TextField->canvas->offset-__HScale,
		.extent = p_TextField->canvas->offset+__HScale
	};
	return p_TextField;
}


// ----------------------------------------------------------------------------------------------------
// Interface Management

/**
 *	create ui system
 *	\param cursor_path: path to cursor texture
 */
UI::UI(const char* cursor_path)
{
	PixelBufferComponent* __CursorTexture = g_Renderer.register_sprite_texture(cursor_path);
	m_CursorSprite = g_Renderer.register_sprite(__CursorTexture,vec2(100,100),vec2(2,50));
}

/**
 *	update ui
 */
void UI::update()
{
	// update cursor animation
	f32 __BlinkFactor = glm::clamp(sin(m_CursorAnim*MATH_PI)*2,-1.,1.);
	m_CursorAnim += UI_CURSOR_BLINK_DELTA;
	m_CursorAnim = fmod(m_CursorAnim,2.f);
	m_CursorSprite->offset = vec2(-100);
	m_CursorSprite->alpha = __BlinkFactor;

	// update ui batches
	for (UIBatch& p_Batch : m_Batches)
	{
		// button updates
		for (Button& p_Button : p_Batch.buttons) p_Button.update();

		// text field updates
		bool __SwitchedFields = false;
		for (TextField& p_TextField : p_Batch.tfields)
			p_TextField.update(p_Batch.font,m_CursorSprite,__SwitchedFields);
	}
}

/**
 *	create an ui batch
 *	\param font: button label and text box font for created batch
 *	\param alpha: (default 1.f) batch transparency
 */
lptr<UIBatch> UI::add_batch(Font* font,f32 alpha)
{
	m_Batches.push_back({
			.font = font,
			.alpha = alpha
		});
	return std::prev(m_Batches.end());
}

/**
 *	remove an ui batch
 *	\param batch: batch target by removal
 */
void UI::remove_batch(lptr<UIBatch> batch)
{
	// remove components
	for (Button& p_Button : batch->buttons) p_Button.remove();
	for (TextField& p_TextField : batch->tfields) p_TextField.remove();

	// finish by removing batch from memory & unsetting input mode
	Input::unset_input_mode();
	m_Batches.erase(batch);
}
