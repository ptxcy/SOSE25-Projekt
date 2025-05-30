#include "ui.h"


// ----------------------------------------------------------------------------------------------------
// Interface Batch

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
	p_Button->canvas = g_Renderer.register_sprite(p_Button->idle,position,scale,.0f,1.f,alignment);

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
	p_TextField->canvas = g_Renderer.register_sprite(p_TextField->idle,position,scale,.0f,1.f,alignment);

	// setup content draw
	vec2 __HScale = scale*vec2(.5f);
	vec2 __TPos = p_TextField->canvas->offset-vec2(__HScale.x*.95f,__HScale.y*.4f);
	p_TextField->content = g_Renderer.write_text(font,"",__TPos,scale.y*.6,vec4(1),
												 { .align=SCREEN_ALIGN_BOTTOMLEFT });

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
 *	update ui
 */
void UI::update()
{
	for (UIBatch& p_Batch : m_Batches)
	{
		// button updates
		for (Button& p_Button : p_Batch.buttons)
		{
			bool __Intersect = p_Button.bounds.intersect(g_Input.mouse.position);

			// button confirmation
			if (__Intersect&&g_Input.mouse.buttons[0])
			{
				g_Renderer.assign_sprite_texture(p_Button.canvas,p_Button.action);
				p_Button.holding = true;
				continue;
			}

			// button hover
			if (__Intersect)
			{
				g_Renderer.assign_sprite_texture(p_Button.canvas,p_Button.hover);
				p_Button.confirm = p_Button.holding;
			}

			// reset button state
			else g_Renderer.assign_sprite_texture(p_Button.canvas,p_Button.idle);
			p_Button.holding = false;
		}

		// text field updates
		bool __SwitchedFields = false;
		for (TextField& p_TextField : p_Batch.tfields)
		{
			bool __Intersect = p_TextField.bounds.intersect(g_Input.mouse.position);

			// user requests to write to text field buffer & clickout handling
			if (p_TextField.active)
			{
				if (!__Intersect&&g_Input.mouse.buttons[0])
				{
					if (!__SwitchedFields) Input::unset_input_mode();
					p_TextField.active = false;
				}
				else p_TextField.content->load_buffer();
			}
			else if (__Intersect&&g_Input.mouse.buttons[0])
			{
				g_Renderer.assign_sprite_texture(p_TextField.canvas,p_TextField.select);
				g_Input.set_input_mode(&p_TextField.content->data);
				p_TextField.active = true;
				__SwitchedFields = true;
			}

			// handle hover over inactive text field & reset to idle if no intersection
			else if (__Intersect) g_Renderer.assign_sprite_texture(p_TextField.canvas,p_TextField.hover);
			else g_Renderer.assign_sprite_texture(p_TextField.canvas,p_TextField.idle);
		}
	}
}

/**
 *	create an ui batch
 *	\param font: button label and text box font for created batch
 */
lptr<UIBatch> UI::add_batch(Font* font)
{
	m_Batches.push_back({ .font = font });
	return std::prev(m_Batches.end());
}

/**
 *	remove an ui batch
 *	\param batch: batch target by removal
 */
void UI::remove_batch(lptr<UIBatch> batch)
{
	// remove button components
	for (Button& p_Button : batch->buttons)
	{
		Renderer::delete_sprite(p_Button.canvas);
		g_Renderer.delete_text(p_Button.label);
	}

	// remove text field components
	for (TextField& p_TextField : batch->tfields)
	{
		Renderer::delete_sprite(p_TextField.canvas);
		g_Renderer.delete_text(p_TextField.content);
	}

	// finish by removing batch from memory
	m_Batches.erase(batch);
}
