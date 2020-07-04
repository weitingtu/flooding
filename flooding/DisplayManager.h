#pragma once

class DisplayManager 
{
public:
	static DisplayManager& get_instance()
	{
		static DisplayManager inst;
		return inst;
	}

	DisplayManager():
    _indivisual_flooding(false),
	_indivisual_flooding_idx(0),
	_total_flooding(false),
    _indivisual_pred(false),
	_indivisual_pred_idx(0),
	_total_pred(false),
	_source_target_only(false),
	_show_grid_line(true)
	{
	}

	void set_indivisual_flooding(bool b) { _indivisual_flooding = b; }
	void set_indivisual_flooding_idx(int i) { _indivisual_flooding_idx = i; }
	void set_total_flooding(bool b) { _total_flooding = b; }
	void set_indivisual_pred(bool b) { _indivisual_pred = b; }
	void set_indivisual_pred_idx(int i) { _indivisual_pred_idx = i; }
	void set_total_pred(bool b) { _total_pred = b; }
	void set_source_termianl_only(bool b) { _source_target_only = b; }
	void set_show_grid_line(bool b) { _show_grid_line = b; }


	bool is_indivisual_flooding() const { return _indivisual_flooding; }
	int get_indivisual_flooding_idx() const { return _indivisual_flooding_idx; }
	bool is_total_flooding() const { return _total_flooding; }
	bool is_indivisual_pred() const { return _indivisual_pred; }
	int get_indivisual_pred_idx() const { return _indivisual_pred_idx; }
	bool is_total_pred() const { return _total_pred; }
	bool is_source_target_only() const { return _source_target_only; }
	bool is_show_grid_line() const { return _show_grid_line;  }

private:
	bool _indivisual_flooding;
	int  _indivisual_flooding_idx;
	bool _total_flooding;
	bool _indivisual_pred;
	int  _indivisual_pred_idx;
	bool _total_pred;
	bool _source_target_only;
	bool _show_grid_line;
};

static DisplayManager& get_display_manager() 
{
	return DisplayManager::get_instance();
}
