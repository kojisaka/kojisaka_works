package com.example.demo;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class FormObj {

	protected String name;
	protected String type;
	protected String detail;

	public void clear() {
		detail = type = name = "";
	}
}
