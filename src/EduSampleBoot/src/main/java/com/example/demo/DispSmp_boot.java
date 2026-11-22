package com.example.demo;

import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.PostMapping;

@Controller
public class DispSmp_boot {

	@PostMapping("/dispSmp")
	public String disp() {
		return "dispSmp.html";
	}
}
