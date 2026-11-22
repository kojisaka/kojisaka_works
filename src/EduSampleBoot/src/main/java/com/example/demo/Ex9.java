package com.example.demo;

import java.util.ArrayList;
import java.util.List;

import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.SessionAttributes;

@Controller
@SessionAttributes("name_tbl")
public class Ex9 {

	@ModelAttribute("name_tbl")
	public List<String> initStorage() {
		return new ArrayList<String>();
	}

	@GetMapping("/Ex9")
	public String get() {
		return "regNoMsg.html";
	}

	@PostMapping("/Ex9")
	public String post
		(@ModelAttribute("name_tbl") List<String> nameTbl,
		 @RequestParam String name) {
		nameTbl.add(name);
		return "regNoMsg.html";
	}
}
