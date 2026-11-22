package com.example.demo;

import java.util.List;

public class FCDepStatus extends DepStatus {

	protected List<String> chains;

	FCDepStatus(String n, String t, List<String> cs) {
		super(n, t);
		chains = cs;
	}

	public String getStatus() {
		String status = name + "　FC　店一覧: ";
		for (String chain: chains) {
			status += (chain + " ");
		}
		return status;
	}
}
