package com.example.demo;

import java.util.ArrayList;
import java.util.List;

import org.springframework.data.domain.Sort;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.PostMapping;

import com.example.demo.entity.DepStatusEntity;

import lombok.RequiredArgsConstructor;

@Controller
@RequiredArgsConstructor
public class DispDB_JPA {

	private final JpaRepository<DepStatusEntity, Integer> repository;

	@PostMapping("/dispDB_JPA")
	public String disp(Model m) {
		List<String> rl = new ArrayList<String>();
		List<DepStatusEntity> dl
			= repository.findAll(Sort.by("name", "type", "shared"));
		if (! dl.isEmpty()) {
			DepStatusEntity pd = null;
			String detail = null;
			for (DepStatusEntity d: dl) {
				if (pd == null) {
					detail = d.getShared();
				} else if (! d.getName().equals(pd.getName()) ||
						   ! d.getType().equals(pd.getType())   ) {
					rl.add(pd.getName() + "　"
							+ pd.getType() + "　" + detail);
					detail = d.getShared();
				} else {
					detail += ("," + d.getShared());
				}
				pd = d;
			}
			rl.add(pd.getName() + "　"
					+ pd.getType() + "　" + detail);
		}
		m.addAttribute("rslt_list", rl);
		return "dispDB_JPA.html";
	}
}
