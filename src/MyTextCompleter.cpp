#include "MyTextCompleter.h"

MyTextCompleter::MyTextCompleter(wxComboBox *textctrl)
{
	m_entry = textctrl;
}

bool MyTextCompleter::Start(const wxString& prefix)
{
	long from, to;
    m_entry->GetSelection(&from, &to);

	if ( to == from )
	{
        from = m_entry->GetLastPosition(); // Take all if no selection.
	}

	const wxString find(m_entry->GetRange(0, from).Upper());
	wxString Match;
	int n = m_entry->GetCount();

	m_index = 0;
	m_completions.clear();

	for(int i=0;i<n;++i)
	{
		wxString str(m_entry->GetString(i));
		size_t pos = str.Upper().find(find);
		if (pos != wxNOT_FOUND)
		{
			m_completions.Add(str);
			if (pos == 0)
				Match = str;
		}
	}
	if (m_completions.empty())
		return false;

	size_t find_pos = 0;
	if (m_completions.Count() == 1)
	{
		wxString str(m_completions[0]);
		find_pos = str.Upper().find(find);
		m_entry->ChangeValue(str);
	}
	else if (!Match.IsEmpty())
		m_entry->ChangeValue(Match.Truncate(find.size() + 16));

	m_entry->SetSelection(find_pos + find.size(),-1);
	return true;
}

wxString MyTextCompleter::GetNext()
{
   if ( m_index == m_completions.size() )
        return wxString();
    return m_completions[m_index++];
}