package com.example.demo;

public class BusDepStatus extends DepStatus {

	protected int numOfPAX;

	BusDepStatus(String n, String t, int num) {
		super(n, t);
		numOfPAX = num;
	}

	public String getStatus() {
		return name + "　バス　利用者数: " + numOfPAX;
	}
}
