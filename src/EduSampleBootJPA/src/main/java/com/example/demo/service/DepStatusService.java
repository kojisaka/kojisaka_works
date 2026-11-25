package com.example.demo.service;

import java.util.List;

import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import com.example.demo.entity.DepStatusEntityEx;
import com.example.demo.repository.DepStatusRepositoryEx;

import lombok.AllArgsConstructor;
import lombok.Getter;

@Service
@Getter
@AllArgsConstructor
@Transactional
public class DepStatusService {

	final static boolean test = true;

	private final DepStatusRepositoryEx repository;

	public void updateBusDS(String name, String type, String detail) {
		int num = Integer.parseInt(detail);
		List<DepStatusEntityEx> dl
			= repository.findByNameAndType(name, type);
		if (test) try{Thread.sleep(num);}catch(Exception e){}
		DepStatusEntityEx d;
		if (dl.isEmpty()) {
			d = new DepStatusEntityEx();
		} else {
			d = dl.get(0);
			if (detail.matches("[+-].*")) {
				num += Integer.parseInt(d.getShared());
			}
		}
		d.setBasics(name, type, String.valueOf(num));
		repository.save(d);
	}

	public void reorgFCDS(String name, String type, List<String> cs) {
		repository.findByNameAndType(name, type);
		repository.deleteByNameAndType(name, type);
		for (String c: cs) {
			if (test) try{Thread.sleep(2000);}catch(Exception e){}
			DepStatusEntityEx d = new DepStatusEntityEx();
			d.setBasics(name, type, c);
			repository.save(d);
		}
	}
}
