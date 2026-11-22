package com.example.demo;

import java.util.ArrayList;
import java.util.List;

import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.SessionAttributes;

@Controller
@SessionAttributes("name_tbl")
public class Ex9_pr1 {

	@ModelAttribute("name_tbl")
	public List<String> initStorage() {
		return new ArrayList<String>();
	}

	@GetMapping("/Ex9_pr1")
	public String get(Model m) {
		m.addAttribute("name", "");
		m.addAttribute("msg" , "");
		return "regMsg.html";
	}

	@PostMapping("/Ex9_pr1")
	public String post
		(@ModelAttribute("name_tbl") List<String> nameTbl,
		 @RequestParam String name,
		 Model m) {
		String msg;
		if (name.isEmpty() || name.matches(".*[\0-/].*")) {
			msg = "名前の入力が正しくありません！";
		} else {
			nameTbl.add(name);
			msg = name + " を登録しました。";
			name = "";
		}
		m.addAttribute("name", name);
		m.addAttribute("msg" , msg );
		return "regMsg.html";
	}
}
