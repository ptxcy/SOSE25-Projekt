#include "ui.h"


/**
 *	add a button to the batch
 *	\param label: button label writing across button surface
 *	\param tidle: idle texture, in case button is not interacted with this will be displayed
 *	\param thover: hover texture, in case mouse cursor hovers over button and button is not clicked
 *	\param taction: action texture, in case button is clicked and held
 *	\param position: center position of button
 *	\param scale: dimensions of the button
 *	\returns address of button to later read interaction state from
 */
Button* UIBatch::add_button(string label,string tidle,string thover,string taction,vec2 position,vec2 scale)
{
	// graphics setup
	Button* out = buttons.next_free();
	out->idle = g_Renderer.register_sprite_texture(tidle.c_str());
	out->hover = g_Renderer.register_sprite_texture(thover.c_str());
	out->action = g_Renderer.register_sprite_texture(taction.c_str());
	out->canvas = g_Renderer.register_sprite(out->idle,position,scale);

	// intersection boundaries
	vec2 hscale = scale*vec2(.5f);
	out->bounds = {
		.position = position-hscale,
		.extent = position+hscale
	};
	return out;
}
// TODO write a text label across the button surface, this needs text support

/**
 *	update ui
 */
void UI::update()
{
	for (UIBatch* p_Batch : batches)
	{
		// button updates
		for (u16 i=0;i<p_Batch->buttons.active_range;i++)
		{
			Button& p_Button = p_Batch->buttons.mem[i];
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
	}
	// TODO check for removed ui batches during iteration
}
